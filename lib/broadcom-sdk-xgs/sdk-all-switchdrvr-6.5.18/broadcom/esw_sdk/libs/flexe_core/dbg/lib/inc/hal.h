

/******************************************************************************
*
* FILE
*
*
* DESCRIPTION
*
*
* NOTE
*
*
******************************************************************************/
#ifndef HAL_H
#define HAL_H


/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "datatype.h"
#include "global_macro.h"

#ifndef SV_TEST
#ifdef OS_LINUX
#include <pthread.h>
#else
#include <semLib.h>
#endif
#endif

/******************************************************************************
*VARIABLE DEFINE
******************************************************************************/
typedef struct _CHIP_REG
{
	unsigned int field_no;
	unsigned int base_addr;
	unsigned int offset_addr;
	unsigned int end_bit;
	unsigned int start_bit;
}CHIP_REG;
typedef struct _CHIP_RAM
{
	UINT_32 base_addr;
	UINT_32 ram_addr;
	UINT_32 width;
}CHIP_RAM;
/*muliti ch alm report*/
typedef struct ch_alm
{
	UINT_32 reg_base_addr;
	UINT_32 reg_start_offset_addr;
	UINT_32 alm_mirror[CH_FIELD];
	
	struct ch_alm *p_ch_alm;
	
	RET_STATUS (*p_ch_alm_report)(UINT_8 chip_id, UINT_8 ch_id, struct ch_alm *ch_alm_inst, UINT_8* status);
	
}CH_ALM;

/******************************************************************************
*LOCAL FUNCTION PROTOTYPE DECLARATION
******************************************************************************/
/* for debug use */
extern const char *bit_start; 
extern const char *bit_middle; /*for debug use*/
extern const char *bit_end;/*for debug use*/

#ifndef SV_TEST
#ifdef OS_LINUX
extern pthread_mutex_t MUTEX_REG_R_W; /*global*/
#else
#endif
#endif

/******************************************************************************
*GLOBAL FUNCTION DEFINITION
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
extern RET_STATUS regp_read(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32* u32_data);
extern RET_STATUS regp_read_print(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr);
extern RET_STATUS regp_write(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32 u32_data);
extern RET_STATUS regp_bit_write(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,
                                 UINT_8 end_bit,UINT_8 start_bit,UINT_32 u32_data);
extern RET_STATUS regp_field_write(UINT_8 chip_id,CHIP_REG chip_reg,UINT_32 u32_data);
extern RET_STATUS regp_bit_read(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,
                                UINT_8 end_bit,UINT_8 start_bit,UINT_32* u32_data);
extern RET_STATUS regp_bit_read_print(UINT_8 chip_id,UINT_32 u32_base_addr, UINT_32 u32_offset_addr,
                                      UINT_8 end_bit, UINT_8 start_bit);
extern RET_STATUS regp_field_read(UINT_8 chip_id,CHIP_REG chip_reg,UINT_32* u32_data);
extern RET_STATUS regp_read_mul(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32 num);
extern void crc_caculate(unsigned int value,unsigned char* b);
extern RET_STATUS ram_write(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_ram_addr,
                            UINT_32 u32_offset_addr,UINT_32 width,UINT_32* data);
extern RET_STATUS ram_write_test(UINT_8 chip_id,UINT_32 u32_addr,UINT_32 offset,
                                 UINT_32 width,UINT_32 parameter1,UINT_32 parameter2,UINT_32 parameter3);
extern RET_STATUS ram_write_single(UINT_8 chip_id,CHIP_RAM chip_ram,UINT_32 offset_addr,UINT_32* data);
extern RET_STATUS ram_write_debug(char * string);
extern RET_STATUS ram_read(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_ram_addr,
                           UINT_32 u32_offset_addr,UINT_32 width,UINT_32* data);
extern RET_STATUS ram_read_single(UINT_8 chip_id,CHIP_RAM chip_ram,UINT_32 offset_addr,UINT_32* data);
extern RET_STATUS ram_read_print(UINT_8 chip_id,UINT_32 u32_ram_addr,UINT_32 u32_offset_addr,UINT_32 width);
extern RET_STATUS ram_read_mul(UINT_8 chip_id,UINT_32 u32_addr,UINT_32 width,UINT_32 num);
extern RET_STATUS regp_mask_write(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,
                                  UINT_8 end_bit,UINT_8 start_bit,UINT_32 u32_data);
extern RET_STATUS regp_mask_write_dword(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,
                                        UINT_32 mask,UINT_32 u32_data);
extern RET_STATUS regp_field_mask_write(UINT_8 chip_id,CHIP_REG chip_reg,UINT_32 u32_data);
extern RET_STATUS address_remap(UINT_8 unit,UINT_8 board_id);
extern RET_STATUS regp_read_for_ram(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32* u32_data);
extern RET_STATUS regp_write_for_ram(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32 u32_data);
extern RET_STATUS ch_alm_report(UINT_8 chip_id, UINT_8 ch_id, CH_ALM *ch_alm_inst, UINT_8* status);
#ifdef __cplusplus
}
#endif
#endif


