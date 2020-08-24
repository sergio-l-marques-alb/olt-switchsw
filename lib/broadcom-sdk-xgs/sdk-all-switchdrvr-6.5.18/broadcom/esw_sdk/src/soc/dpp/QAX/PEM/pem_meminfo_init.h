#ifndef _PEM_MEMINFO_INIT_H_
#define _PEM_MEMINFO_INIT_H_

#include <stdio.h>
#include "pem_logical_access.h"
/*
 * The following function read (and parse) data-bases memory definition.
 * This memory definition is one of the PEM-compiler outputs.
 *
 * This function MUST be called prior to any db access.
 */

#define MAX_NAME_LENGTH 64
#define MAX_MEM_DATA_LENGTH 512

#define START_COMMENT              "/*"
#define END_COMMENT                "*/"
#define COMMENT_MARK_SIZE          2


/**********************************************
 *         Database initial information
 **********************************************/



#define KEYWORD_DB_INFO                                        "DB_INFO"
#define KEYWORD_DB_INFO_SIZE                                   sizeof("DB_INFO") - 1
#define KEYWORD_KEY_INFO                                       "KEY_FIELD"
#define KEYWORD_KEY_INFO_SIZE                                  sizeof("KEY_FIELD") - 1
#define KEYWORD_RESULT_INFO                                    "RESULT_FIELD"
#define KEYWORD_RESULT_INFO_SIZE                               sizeof("RESULT_FIELD") - 1
#define KEYWORD_VIRTUAL_REGISTER_MAPPING                       "VIRTUAL_REGISTER_MAPPING"
#define KEYWORD_REGISTER_INFO_SIZE                             sizeof("VIRTUAL_REGISTER_MAPPING") - 1  
#define KEYWORD_VIRTUAL_REGISTER_NOF_FIELDS                    "VIRTUAL_REGISTER_NOF_FIELDS"
#define KEYWORD_VIRTUAL_REGISTER_NOF_FIELDS_SIZE               sizeof("VIRTUAL_REGISTER_NOF_FIELDS") - 1
#define KEYWORD_VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS            "VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS"
#define KEYWORD_VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS_SIZE       sizeof("VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS") - 1
#define KEYWORD_DB_DIRECT_INFO                                 "DB_DIRECT_MAPPING_INFO"
#define KEYWORD_DB_DIRECT_INFO_SIZE                            sizeof("DB_DIRECT_MAPPING_INFO") - 1
#define KEYWORD_DB_TCAM_KEY_INFO                               "DB_TCAM_KEY_MAPPING_INFO"
#define KEYWORD_DB_TCAM_KEY_INFO_SIZE                          sizeof("DB_TCAM_KEY_MAPPING_INFO") - 1
#define KEYWORD_DB_TCAM_RESULT_INFO                            "DB_TCAM_RESULT_MAPPING_INFO"
#define KEYWORD_DB_TCAM_RESULT_INFO_SIZE                       sizeof("DB_TCAM_RESULT_MAPPING_INFO") - 1
#define KEYWORD_DB_EXACT_MATCH_KEY_INFO                        "DB_EXACT_MATCH_KEY_MAPPING_INFO"
#define KEYWORD_DB_EXACT_MATCH_KEY_INFO_SIZE                   sizeof("DB_EXACT_MATCH_KEY_MAPPING_INFO") - 1
#define KEYWORD_DB_EXACT_MATCH_RESULT_INFO                     "DB_EXACT_MATCH_RESULT_MAPPING_INFO"
#define KEYWORD_DB_EXACT_MATCH_RESULT_INFO_SIZE                sizeof("DB_EXACT_MATCH_RESULT_MAPPING_INFO") - 1
#define KEYWORD_DB_LONGEST_PERFIX_MATCH_KEY_INFO               "DB_LONGEST_PREFIX_MATCH_KEY_MAPPING_INFO"
#define KEYWORD_DB_LONGEST_PERFIX_MATCH_KEY_INFO_SIZE          sizeof("DB_LONGEST_PREFIX_MATCH_KEY_MAPPING_INFO") - 1
#define KEYWORD_REG_AND_DBS_NUM_INFO                          "REG_AND_DBS_NUM_INFO" 
#define KEYWORD_REG_AND_DBS_NUM_INFO_SIZE                     sizeof("REG_AND_DBS_NUM_INFO") - 1
#define KEYWORD_DB_LONGEST_PERFIX_MATCH_RESULT_INFO            "DB_LONGEST_PREFIX_MATCH_RESULT_MAPPING_INFO"
#define KEYWORD_DB_LONGEST_PERFIX_MATCH_RESULT_INFO_SIZE       sizeof("DB_LONGEST_PREFIX_MATCH_RESULT_MAPPING_INFO") - 1


#define KEYWORD_DB_SINGLE_VIRT_DB_MAP_INFO                     "DB_SINGLE_VIRT_DB_MAP_INFO"
#define KEYWORD_DB_SINGLE_VIRT_DB_MAP_INFO_SIZE                sizeof("DB_SINGLE_VIRT_DB_MAP_INFO") - 1
#define KEYWORD_DB_DIRECT                                      "DIRECT"
#define KEYWORD_DB_DIRECT_SIZE                                 sizeof("DIRECT") - 1
#define KEYWORD_DB_TCAM                                        "TCAM"
#define KEYWORD_DB_TCAM_SIZE                                   sizeof("TCAM") - 1
#define KEYWORD_DB_EM                                          "EM"
#define KEYWORD_DB_EM_SIZE                                     sizeof("EM") - 1
#define KEYWORD_DB_LPM                                         "LPM"
#define KEYWORD_DB_LPM_SIZE                                    sizeof("LPM") - 1




/* DB_DIRECT_MAPPING_INFO - line format
 *
 * KEYWORD_DB_DIRECT_INFO                                                  
 * <virtual-mem-name for debug>                                            
 * <db_id>                                                                 
 * <field-name for debug>                                                  
 * <field_id>                                                              
 * <result_field_width_for_debug>                                          
 * <mem-dimensions for debug>     (3 tokens 'rows' 'x' 'columns)           
 * <vc_st_row:vc_end_row>         (3 tokens 'vc_st_col' ':' 'vc_end_row')  
 * <vc_st_col:vc_end_col>         (3 tokens 'vc_st_col' ':' 'vc_end_col')  
 * {MAP|CAM}                      pe-type keyword                          
 * <pe_matrix_col>                                                         
 * <phy_mem_index>                                                         
 * <pem_mem_name> 
 * <pem_mem_address>              ( "40'hxxxx" where the first byte contains the block-id and the 32 LSb are the address ) 
 * <mem_st_row>                                                            
 * <mem_st_col> 
 * <mem_width_in_bits>
 * <implementation_index>
 */                                                                        
#define   PEM_NOF_DB_DIRECT_INFO_TOKEN  24 

/* KEYWORD_DB_INFO - line format
 *
 * DB_INFO
 * <db_type>
 * <Database-name> 
 * <db_id>
 * <nof_entries>
 * <key-width> 
 * <result-width> 
 *<key_field_num>
 * <result_field_num>
 *
 */
#define   PEM_NOF_DB_INFO_TOKEN   9


/* KEYWORD_KEY_INFO - line format
 *
 * KEY_INFO
 * <Database-name for debug> 
 * <db_type>
 * <db id>
 * <key-field-name for debug>
 * <key-field-id>
 * <lsb_bit>
 * <msb_bit>
 */
#define   PEM_NOF_KEY_FIELD_TOKEN  8


/* KEYWORD_RESULT_INFO - line format
 *
 * RESULT_FIELD 
 * <Database-name for debug> 
 * <db_type>
 * <db id>
 * <result-field-name for debug>
 * <result-field-id>
 * <lsb_bit>
 * <msb_bit>
 */
#define   PEM_NOF_RESULT_FIELD_TOKEN  8 


/* KEYWORD_VIRTUAL_REGISTER_MAPPING - line format
 *
 * VIRTUAL_REGISTER_MAPPING  
 * <Register-name for debug>
 * <reg-id>
 * <mapping_id>
 * <register field name for debug>
 * <register-field-id>
 * <register-total-width>
 * <register-field-msb:register-field-lsb>   (3 tokens 'register-field-msb' ':' 'register-field-lsb')
 * <pem-type>
 * <pe-matrix-col>
 * <program-mem-index>
 * <program-mem-name>
 * <pem_mem_addr>                             ( "40'hxxxx" where the first byte contains the block-id and the 32 LSb are the address ) 
 * <prog_mem_line> 
 * <pem_mem_offset> 
 * <pem_mem_width_in_bits>
 */
#define   PEM_NOF_REGISTER_INFO_TOKEN  18


/* KEYWORD_VIRTUAL_REGISTER_NOF_FIELDS - line format
 *
 * KEYWORD_VIRTUAL_REGISTER_NOF_FIELDS 
 * <reg_id>
 * <nof_fields>
 */
#define   VIRTUAL_REGISTER_NOF_FIELDS_TOKEN  3


/* KEYWORD_VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS - line format
 *
 * KEYWORD_VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS
 * <reg_id>
 * <field_id>
 * <nof_mappings>
 */
#define   VIRTUAL_REGISTER_FIELD_NOF_MAPPINGS_TOKEN  4


/* DB_TCAM_KEY_MAPPING_INFO   - line format
 *
 * DB_TCAM_KEY_MAPPING_INFO                                                
 * <virtual-mem-name for debug> 
 * <db_id> 
 * <key-name for debug>  
 * <virt_KEY_mem-total-dimensions for debug>  (3 tokens 'rows' 'x' 'columns')  
 * <vc_end_row:vc_st_row>                     (3 tokens 'end-rows' ':' 'start-rows')
 * <vc_valid_col>  
 * <vc_mask_end_col: vc_mask_st_col >         (3 tokens 'mask-end-col' ':' 'mask-st-col')
 * <vc_key_end_col: vc_key_st_col >           (3 tokens 'key-end-col' ':' 'key-st-col')
 * <pe_type> 
 * <pe_matrix_col> 
 * <phy_mem_ndx> 
 * <pem_mem_name>
 * <pem_mem_addr>                             ( "40'hxxxx" where the first byte contains the block-id and the 32 LSb are the address )  
 * <mem_st_row> 
 * <mem_valid_col> 
 * <mem_mask_st_col> 
 * <mem_key_st_col>
 * <mem_width_in_bits>
 * <implementation_index>
 */                                                                        
#define   PEM_NOF_DB_CAM_KEY_MAPPING_INFO_TOKEN  28 

/* DB_TCAM_RESULT_MAPPING_INFO    - line format
 *
 * DB_TCAM_RESULT_MAPPING_INFO
 * <virtual-mem-name for debug> 
 * <db_id> 
 * <result-name for debug>  
 * <virt_RESULT_mem-dimensions for debug>      (3 tokens 'rows' 'x' 'columns') 
 * <vc_end_row:vc_st_row>                      (3 tokens 'end-rows' ':' 'st-rows')
 * <vc_end_col: vc_st_col >                    (3 tokens 'end-columns' ':' 'st-columns') 
 * <pe_type> 
 * <pe_matrix_col>
 * <phy_total_width_in_bits>
 * <phy_mem_ndx> 
 * <pem_mem_name> 
 * <pem_mem_addr>                             ( "40'hxxxx" where the first byte contains the block-id and the 32 LSb are the address )
 * <mem_st_row> 
 * <mem_st_col> 
 * <implementation_index>
 *                                           
 */                                                                        
#define   PEM_NOF_DB_CAM_RESULT_MAPPING_INFO_TOKEN  22 
 

/* DB_SINGLE_VIRT_DB_MAP_INFO               -line format
*
* DB_SINGLE_VIRT_DB_MAP_INFO
* <db_id>
* <nof_chunk_rows>
* <nof_chunk_cols>
* <nof_implementations>
*
*/
#define   DB_SINGLE_VIRT_DB_MAP_INFO_DIRECT_TOKEN 5


/* DB_SINGLE_VIRT_DB_MAP                    -line format
*
* DB_SINGLE_VIRT_DB_MAP_INFO
* <db_id>
* <nof_key_chunk_rows>
* <nof_key_chunk_cols>
* <nof_result_chunk_rows>
* <nof_result_chunk_cols>
* <nof_implementations>S
*
*/
#define   DB_SINGLE_VIRT_DB_MAP_INFO_TOKEN 7





/**********************************************
 *         uCode definition
 **********************************************/

#define KEYWORD_UCODE_PEM_START                   "PEM_START"
#define KEYWORD_UCODE_PEM_START_SIZE              sizeof("PEM_START") - 1
#define KEYWORD_UCODE_PEM_END                     "PEM_END"
#define KEYWORD_UCODE_PEM_END_SIZE                sizeof("PEM_END") - 1
#define KEYWORD_UCODE_REG_LINE_START_INFO         "REG_LINE_START"
#define KEYWORD_UCODE_REG_LINE_START_INFO_SIZE    sizeof("REG_LINE_START") - 1
#define KEYWORD_UCODE_REG_LINE_END                "REG_LINE_END"
#define KEYWORD_UCODE_REG_LINE_END_SIZE           sizeof("REG_LINE_END") - 1 
#define KEYWORD_UCODE_FIELD_INFO                  "FIELD"
#define KEYWORD_UCODE_FIELD_INFO_SIZE             sizeof("FIELD") - 1
#define KEYWORD_UCODE_MEM_LINE_START_INFO         "MEM_LINE_START"
#define KEYWORD_UCODE_MEM_LINE_START_INFO_SIZE    sizeof("MEM_LINE_START") - 1
#define KEYWORD_UCODE_MEM_LINE_END                "MEM_LINE_END"
#define KEYWORD_UCODE_MEM_LINE_END_SIZE           sizeof("MEM_LINE_END") - 1

/* PEM_START - line format
 * 
 * PEM_START
 * <pe_type_name>
 * <pem_row> 
 * <pem_col>
 */

#define PEM_NOF_PEM_START_TOKEN 4

/* PEM_END - line format
 *
 * PEM_END
 */
#define PEM_NOF_PEM_END_TOKEN 1

/* REG_LINE_START - line format
 * 
 * REG_LINE_START 
 * <reg_name> 
 * <reg_addr>                     ( "40'hxxxx" where the first byte contains the block-id and the 32 LSb are the address )
 * <reg_val>                      ( "n'hxxxxxxx" format )
 */

#define PEM_NOF_REG_LINE_START_TOKEN 4  /*changed from 9*/

/* REG_LINE_END - line format
 *
 * REG_LINE_END
 */
#define PEM_NOF_REG_LINE_END_TOKEN 1

/* FIELD - line format
 * 
 * FIELD 
 * <field_id_0> 
 * <field_name_0> 
 * <field_val_0>             ( "n'hxxxxxxx" format )
 */
#define PEM_NOF_FIELD_TOKEN 4

/* MEM_LINE_START - line format
 * 
 * MEM_LINE_START 
 * <pem_mem_name> 
 * <pem_mem_addr>                     ( "40'hxxxx" where the first byte contains the block-id and the 32 LSb are the address )
 * <prog_mem_address>   
 * <all_line_bits_val>                ( "n'hxxxxxxx" format )
 */
#define PEM_NOF_MEM_LINE_START_TOKEN 5

/* MEM_LINE_END - line format
 *
 * MEM_LINE_END
 */
#define PEM_NOF_MEM_LINE_END_TOKEN 1



#define phy_mem_len 6    /* 7 = pe_type (1 bit, MAP or CAM) + pe_index (4 bits, max of 16 PEs) + sram_index (2 bits, max of 4 SRAM) */
#define phy_mem2mem_index(phy_mem)  (phy_mem & (0xFFFFFFFF >> (32 - phy_mem_len)))
#define build_mem_index(db_type, pe_index, sram_index) (db_type | (pe_index << 1) | (sram_index << 5))






/**********************************************
 *         EDI's function definitions
 **********************************************/

/*Insert memory line*/
void qax_mem_line_insert(const char* line);
/*Insert register line*/
void qax_reg_line_insert(const char* line);

/* Inserts info of Db by ID*/
void qax_db_info_insert(const char* line);
/* Insert msb/lsb mapping*/
void qax_db_field_info_insert(const char* line);
/* Insert msb/lsb mapping to logical_key_fields_location*/
void qax_db_key_field_info_insert(const char* db_type, const int db_id, const int field_id, const int lsb_bit, const int msb_bit);
/* Insert msb/lsb mapping to logical_result_fields_location*/
void qax_db_result_field_info_insert(const char* db_type, const int db_id, const int field_id, const int lsb_bit, const int msb_bit);

/* Insert one chunk into LogicalDirectMapper*/
void qax_direct_result_chunk_insert(const char* line);
/* Insert one CAM based key chunk into api_info.db_tcam_container.db_tcam_info_arr*/
void qax_tcam_key_chunk_insert(const char* line);
/* Insert one CAM based result chunk into api_info.db_tcam_container.db_tcam_info_arr*/
void qax_tcam_result_chunk_insert(const char* line);
/*Insert one key chunk of EM database into api_info.db_em_container.db_em_info_arr*/
void qax_em_key_chunk_insert(const char* line);
/*Insert one key chunk of EM database into api_info.db_em_container.db_em_info_arr*/
void qax_em_result_chunk_insert(const char* line);
/*Insert one key chunk of EM database into api_info.db_lpm_container.db_lpm_info_arr*/
void qax_lpm_key_chunk_insert(const char* line);
/*Insert one key chunk of EM database into api_info.db_lpm_container.db_lpm_info_arr*/
void qax_lpm_result_chunk_insert(const char* line);

/*Insert register*/
void qax_register_insert(const char* line);

/* Build result chunk of DIRECT db. Modifies chunk_info_ptr and returns db_id*/
unsigned int qax_build_direct_chunk_from_ucode(const char* line, DbChunkMapper* chunk_info_ptr);
/* Build key chunk of CAM based db. Modifies chunk_info_ptr and returns db_id*/
unsigned int qax_build_cam_key_chunk_from_ucode(const char* line, DbChunkMapper* chunk_info_ptr);
/* Build result chunk of CAM based db. Modifies chunk_info_ptr and returns db_id*/
unsigned int qax_build_cam_result_chunk_from_ucode(const char* line, DbChunkMapper* chunk_info_ptr);
/* Build register field mapping info*/
unsigned int qax_build_register_mapping_from_ucode(const char* line, struct RegFieldMapper* reg_field_mapping_ptr);

/* Insert the chunk into right place in the list*/
void qax_db_chunk_insert(LogicalDbChunkMapperMatrix* logical_db_mapper, DbChunkMapper*const chunk_info_ptr);

void qax_db_virtual_reg_field_mapping_insert(struct LogicalRegInfo* reg_info_p, struct RegFieldMapper* reg_field_mapping_ptr);

/* Gets size of each DB from line and initialize it*/
void qax_init_all_db_arr_by_size(const char* line);
/* Init DbChunkMapper for a single Db_id*/
void qax_init_logical_db_chunk_mapper(const char* line);
/* Init LogicalDbChunkMapperMatrix mattrix*/
void qax_init_logical_db_mapper_matrix(LogicalDbChunkMapperMatrix** logical_db_mapper_matrix_ptr, const int nof_chunk_rows, const int nof_chunk_cols, const int nof_implementations);
/* Init msb/lsb bits mattrix*/
void qax_init_logical_fields_location(FieldBitRange** field_bit_range_arr, const int nof_fields);
/* Init Logical_Reg_Field_Info*/
void qax_init_reg_field_info(const char* line);
/* Init Reg_Field_Mapper*/
void qax_init_reg_field_mapper(const char* line);



unsigned int qax_hexstr2addr(char *str, unsigned int *block_id);



#endif /* _PEM_MEMINFO_INIT_H_ */
