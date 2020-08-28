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



/******************************************************************************
*HISTORY OF CHANGES
*******************************************************************************
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-1-3      1.0           initial
*
******************************************************************************/
#ifndef BC_1588_CPU_H
#define BC_1588_CPU_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "datatype.h"


/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/



/******************************************************************************
*VARIABLE DEFINE
******************************************************************************/



/******************************************************************************
*LOCAL FUNCTION PROTOTYPE DECLARATION
******************************************************************************/


/******************************************************************************
*GLOBAL FUNCTION DEFINITION
******************************************************************************/
#ifdef __cplusplus
extern "C" 
{
#endif
extern RET_STATUS bc_cpu_c2p_wdata_set(UINT_8 chip_no ,UINT_32 parameter);
extern RET_STATUS bc_cpu_c2p_wdata_get(UINT_8 chip_no ,UINT_32 *parameter);
extern RET_STATUS bc_cpu_c2p_finish_set(UINT_8 chip_no ,UINT_32 parameter);
extern RET_STATUS bc_cpu_p2c_finish_set(UINT_8 chip_no ,UINT_32 parameter);
extern RET_STATUS bc_cpu_c2p_frm_cnt_get(UINT_8 chip_no ,UINT_32 *parameter);
extern RET_STATUS bc_cpu_p2c_frm_cnt_get(UINT_8 chip_no ,UINT_32 *parameter);
extern RET_STATUS bc_cpu_c2p_cstate_get(UINT_8 chip_no ,UINT_32 *parameter);
extern RET_STATUS bc_cpu_c2p_bank_full_get(UINT_8 chip_no ,UINT_32 *parameter);
extern RET_STATUS bc_cpu_p2c_rdata_get(UINT_8 chip_no ,UINT_32 *parameter);
extern RET_STATUS bc_cpu_p2c_cstate_get(UINT_8 chip_no ,UINT_32 *parameter);
extern RET_STATUS bc_cpu_p2c_bank_empty_get(UINT_8 chip_no ,UINT_32 *parameter);
extern RET_STATUS bc_cpu_c2p_len_err_get(UINT_8 chip_no ,UINT_32 *parameter);
extern RET_STATUS bc_cpu_c2p_dscp_err_get(UINT_8 chip_no ,UINT_32 *parameter);
extern RET_STATUS bc_cpu_c2p_ram_read_pkt_err_get(UINT_8 chip_no ,UINT_32 *parameter);
extern RET_STATUS bc_cpu_p2c_ptp_err_get(UINT_8 chip_no ,UINT_32 *parameter);
extern RET_STATUS bc_cpu_p2c_len_err_get(UINT_8 chip_no ,UINT_32 *parameter);
extern RET_STATUS bc_cpu_cnt_get(UINT_8 chip_no, UINT_8 sel, UINT_32 *parameter);

#ifdef __cplusplus
}
#endif

#endif


