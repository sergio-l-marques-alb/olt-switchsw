#ifndef _PEM_LOGICAL_ACCESS_H_
#define _PEM_LOGICAL_ACCESS_H_

/* The following definition 'DB_OBJECT_ACCESS' is to compile code accessing Kidron DB objects (C++).
 * In case of using Kidron outputs (not the C++ environment) it should be commented out.
 */

/* #define DB_OBJECT_ACCESS */

#include <limits.h>
#include "pem_physical_access.h"

/* Error codes */
typedef enum {
  PEM_ERR_NONE                 =  0,
  PEM_ERR_INTERNAL             = -1,
  PEM_ERR_MEMORY               = -2,
  PEM_ERR_UNIT                 = -3,
  PEM_ERR_PARAM                = -4,
  PEM_ERR_EMPTY                = -5,
  PEM_ERR_FULL                 = -6,
  PEM_ERR_NOT_FOUND            = -7,
  PEM_ERR_EXISTS               = -8,
  PEM_ERR_TIMEOUT              = -9,
  PEM_ERR_BUSY                 = -10,
  PEM_ERR_FAIL                 = -11,
  PEM_ERR_DISABLED             = -12,
  PEM_ERR_BADID                = -13,
  PEM_ERR_RESOURCE             = -14,
  PEM_ERR_CONFIG               = -15,
  PEM_ERR_UNAVAIL              = -16,
  PEM_ERR_INIT                 = -17,
  PEM_ERR_PORT                 = -18,
  PEM_ERR_LIMIT                = -19           
} pem_err_code_t;

/* PEM logical access layer */

typedef unsigned int table_id_t;   
typedef unsigned int field_id_t; 
typedef unsigned int reg_id_t;  
typedef unsigned int tcam_id_t;

typedef struct pem_field_access_s {
	  field_id_t	  field;    /* Field ID */
    uint32*       fldbuf;
    uint32  	    flags;    
} pem_field_access_t;

typedef struct pem_mem_access_s {	
    uint16                  nFields;
    pem_field_access_t      *fields; /* Array of data fields */
    uint32  	              flags;    
} pem_mem_access_t;


typedef struct DbChunkMapper {
  unsigned int           phy_pe_matrix_col;
  unsigned int           phy_mem_index;
  char*                  phy_mem_name;
  unsigned int           phy_mem_addr;
  unsigned int           phy_mem_width;
  unsigned int           mem_block_id;   
  unsigned int           phy_mem_entry_offset;
  unsigned int           phy_mem_width_offset;
  unsigned int           virtual_mem_entry_offset;
  unsigned int           virtual_mem_width_offset;
  unsigned int           chunk_width;
  unsigned int           chunk_entries;
  unsigned int           chunk_matrix_row_ndx;
  unsigned int           chunk_matrix_col_ndx;
  unsigned int           chunk_matrix_ndx;
} DbChunkMapper;


typedef struct LogicalDbChunkMapperMatrix {
  DbChunkMapper*** db_chunk_mapper; /* matrix of pointers, NULL pointer means that the entry in the matrix is not initialized. common for TCAM key, TCAM result & PeMap Sram */
} LogicalDbChunkMapperMatrix;

typedef struct EmDbCache {
  unsigned int** em_key_entry_arr; /* array of entries, each entry contains an array that represent a single line (line width can be bigger than 32 bits, so it is possible that each entry requires few ints)*/
}EmDbCache;

typedef struct LpmDbCache {
  unsigned int** lpm_key_entry_arr; /* array of entries, each entry contains an array that represent a single line (line width can be bigger than 32 bits, so it is possible that each entry requires few ints) */
  unsigned int*  lpm_key_entry_prefix_length_arr;
}LpmDbCache;


typedef struct FieldBitRange {
  unsigned int          lsb;
  unsigned int          msb;
}FieldBitRange;

typedef struct LogicalDirectInfo {
  unsigned int                         total_nof_entries;
  unsigned int                         total_result_width;
  unsigned int                         nof_fields;
  FieldBitRange*                       field_bit_range_arr;    /* array of fields, in each entry (lsb, msb) of the field*/
  LogicalDbChunkMapperMatrix*          result_chunk_mapper_matrix_arr;
  unsigned int                         nof_chunk_matrices;
} LogicalDirectInfo;

typedef struct LogicalTcamInfo {
  unsigned int                           total_nof_entries;
  unsigned int                           total_key_width;
  unsigned int                           total_result_width;
  unsigned int                           nof_fields_in_key;
  unsigned int                           nof_fields_in_result;
  FieldBitRange*                         key_field_bit_range_arr;
  FieldBitRange*                         result_field_bit_range_arr;
  LogicalDbChunkMapperMatrix*            key_chunk_mapper_matrix_arr;
  LogicalDbChunkMapperMatrix*            result_chunk_mapper_matrix_arr;
  unsigned int                           nof_chunk_matrices;
} LogicalTcamInfo;

typedef struct LogicalEmInfo {
  LogicalTcamInfo               logical_em_info;
  EmDbCache                     em_cache;
} LogicalEmInfo;

typedef struct LogicalLpmInfo {
  LogicalTcamInfo               logical_lpm_info;
  LpmDbCache                    lpm_cache;
} LogicalLpmInfo;


/* The following map a piece of a logical field to a memory chunk (or part of it) of PEM-memory */
typedef struct RegFieldMapper {
  /* virtual field mapping info*/
  unsigned int                 mapping_id                           ; /* per reg-field there are N mappings with ids 0..N-1*/
  unsigned int                 field_id                             ; /* for debug - it is the location in the containing array*/
  unsigned int                 cfg_mapper_width                     ; /* the width of m_wire_cfg_p that this cCfgMapper is implementing*/
  unsigned int                 virtual_field_lsb                    ;
  unsigned int                 virtual_field_msb                    ;
  /* physical field mapping info*/
  unsigned int                 pem_matrix_col                       ;
  unsigned int                 pem_mem_block_id                     ;
  unsigned int                 pem_mem_address                      ;
  unsigned int                 pem_mem_line                         ;
  unsigned int                 pem_mem_offset                       ;
  unsigned int                 pem_mem_total_width                  ;
} RegFieldMapper; 

typedef struct LogicalRegFieldInfo {
  unsigned int                 nof_mappings;
  RegFieldMapper**    reg_field_mapping_arr;
} LogicalRegFieldInfo;


typedef struct LogicalRegInfo {
  unsigned int nof_fields; 
  LogicalRegFieldInfo* reg_field_info;
} LogicalRegInfo;


typedef struct LogicalRegContainer {
  unsigned int nof_registers;
  LogicalRegInfo* reg_info_arr;
} LogicalRegContainer;


typedef struct LogicalDirectContainer {
  unsigned int         nof_direct_dbs;
  LogicalDirectInfo*   db_direct_info_arr;
} LogicalDirectContainer;

typedef struct LogicalTcamContainer {
  unsigned int         nof_tcam_dbs;
  LogicalTcamInfo*     db_tcam_info_arr;
} LogicalTcamContainer;

typedef struct LogicalEmContainer { 
  unsigned int          nof_em_dbs;
  LogicalEmInfo*        db_em_info_arr;
} LogicalEmContainer;

typedef struct LogicalLpmContainer { 
  unsigned int           nof_lpm_dbs;
  LogicalLpmInfo*        db_lpm_info_arr;
} LogicalLpmContainer;

typedef struct ApiInfo { 
  LogicalDirectContainer db_direct_container;
  LogicalTcamContainer   db_tcam_container;
  LogicalEmContainer     db_em_container;
  LogicalLpmContainer    db_lpm_container;
  LogicalRegContainer    reg_container;
} ApiInfo;


/*serves as an element in a list that maintains write data*/
typedef struct PhysicalWriteInfo {
  phy_mem_t                   mem;
  unsigned int*               entry_data;
  struct PhysicalWriteInfo*   next;
} PhysicalWriteInfo;

/* Init 
 * This function is responsible of linking between logical names to physical memory location.
 * Note that this function MUST be called before any memory access.
 *
 * Parameters:
 * 1. file-name - file name (including the path) of the Kidron output file containing the PEM memory definitions.
 *                In case of null the define 'PEM_DEFAULT_DB_MEMORY_MAP_FILE' is taken
 */
#define PEM_DEFAULT_DB_MEMORY_MAP_FILE  "pem_memory_map.txt"

int pem_meminfo_init(const char *vmem_definition_file_name /* in case of null, PEM_DEFAULT_DB_MEMORY_MAP_FILE is taken */); 

/* Read/Write function to (logical) access the PEM Logical Db Direct Data-Bases */
int pem_logical_db_direct_write(int unit, table_id_t db_id, uint32 row_index, pem_mem_access_t *data);
int pem_logical_db_direct_read (int unit, table_id_t db_id, uint32 row_index, pem_mem_access_t *result);

/* Read/Write function to (logical) access the PEM Logical Registers */
int pem_logical_reg_write(int unit, reg_id_t reg_id, pem_mem_access_t *data);
int pem_logical_reg_read(int unit,  reg_id_t reg_id, pem_mem_access_t *result);

/* Read/Write function to (logical) access the PEM Logical DB TCAM  */
int pem_logical_db_tcam_write(int unit, tcam_id_t tcam_id, uint32 row_index, pem_mem_access_t *key, pem_mem_access_t *mask, pem_mem_access_t *valid, pem_mem_access_t *data);
int pem_logical_db_tcam_read(int unit,  tcam_id_t tcam_id, uint32 row_index, pem_mem_access_t *key, pem_mem_access_t *mask, pem_mem_access_t *valid, pem_mem_access_t *result);


/*
 * The following functions are higher logical access to logical Databases.
 * Which means that user do not know how actually this memory are implemented (sram/tcam,...)
 * Implementation of those function are using the above memories access.
 *
 * All the following function return '0' on success or negative number indicating error.
 */

/* EM (Exact Match) access functions */
/*************************************/

/* add entry, 
 * returns the index (in 'index' parameter) in the EM table
 * The function return '0' on success, or  number indicating error:
 * - key already exists returns error and the index of existed entry, 
 * - table is full 
 */
int pem_em_insert(int unit, table_id_t table_id,  pem_mem_access_t* key,  pem_mem_access_t* result, int *index);

/* remove entry, 
 * returns the index (in 'index'  parameter) in the EM table that was cleared. 
 * The function return '0' on success or  number indicating error:
 * - entry not exists.
 */
int pem_em_remove(int unit, table_id_t table_id,  pem_mem_access_t* key, int *index); 

/* lookup entry, 
 * returns the index (in 'index' parameter) in the EM table that was found, the key and the result.
 * Function return '0' on success or number indicating error:
 */
int pem_em_lookup(int unit, table_id_t table_id,  pem_mem_access_t* key, pem_mem_access_t* result, int *index);

/* get next EM entry, 
 * returns the index (in 'index' parameter) in the EM table of the next valid entry, 
 * to receive the first entry use index=-1, if no more entries exists returns index=-1.
 * Return '0' on success, or number indicating error:
 */
int pem_em_get_next_index(int unit, table_id_t table_id, int* index);

/* get EM entry key and result by index, 
 * Return '0' on success, or number indicating error:
 * - entry not valid 
 */
int pem_em_entry_get_by_id(int unit, table_id_t table_id,  int index,  pem_mem_access_t* key, pem_mem_access_t* result);

/* set EM entry key and result by index, override existing entry. 
 * Return '0' on success, or number indicating error:
 */
int pem_em_entry_set_by_id(int unit, table_id_t table_id,  int index, pem_mem_access_t* key, pem_mem_access_t* result);

/* remove all EM entries in DB */
int pem_em_remove_all(int unit, table_id_t table_id);

/* remove EM entry by index (invalidate).
 * Return '0' on success, or number indicating error:
 * - not exists.
 */
int pem_em_remove_by_index (int unit, table_id_t table_id,  int index);


/* LPM (Longest Prefix Match) access functions */ 
/***********************************************/

/* add entry, 
 * returns the index (in 'index' parameter) in the LPM table.
 * The function return '0' on success, or  number indicating error:
 * - key already exists returns error and the index of existed entry, 
 * - table is full 
 */
int pem_lpm_insert(int unit, table_id_t table_id,  pem_mem_access_t* key,  int length_in_bits,  pem_mem_access_t* result, int *index);

/* remove entry, 
 * returns the index (in 'index'  parameter) in the LPM table that was cleared. 
 * The function return '0' on success or  number indicating error:
 * - entry not exists.
 */
int pem_lpm_remove(int unit, table_id_t table_id,  pem_mem_access_t* key,  int length_in_bits, int *index);

/* lookup entry, 
 * returns the index (in 'index' parameter) in the LPM table that was found, the key and the result.
 * Function return '0' on success or number indicating error:
 */
int pem_lpm_lookup(int unit, table_id_t table_id,  pem_mem_access_t* key,  pem_mem_access_t* result, int *index);

/* get next entry, 
 * returns the index (in 'index' parameter) in the LPM table of the next valid entry, 
 * to receive the first entry use index=-1, if no more entries exists returns index=-1.
 * Return '0' on success, or number indicating error:
 */
int pem_lpm_get_next_index(int unit, table_id_t table_id, int* index);

/* get LPM entry key and result by index, 
 * Return '0' on success, or number indicating error:
 * - entry not valid 
 */
int pem_lpm_get_by_id (int unit, table_id_t table_id,  int index,  pem_mem_access_t* key, int *length_in_bits, pem_mem_access_t* result);

/* set LPM entry key and result by index.
 * override existing entry. 
 * Return '0' on success, or number indicating error:
 */
int pem_lpm_set_by_id (int unit, table_id_t table_id,  int index, pem_mem_access_t* key, int *length_in_bits, pem_mem_access_t* result);

/*remove all LPM entries in DB */
int pem_lpm_remove_all(int unit, table_id_t table_id);

/* remove LPM entry by index (invalidate).
 * Return '0' on success, or number indicating error:
 * - not exists.
 */
int pem_lpm_remove_by_index (int unit, table_id_t table_id,  int index);

/* MBIST */ 
/***********************************************/
/*perform virtual MBIST to pem dbs - write then read content to verify correctness*/
int pem_mem_virtual_bist();

#endif /* _PEM_LOGICAL_ACCESS_H_ */

