

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int top_para_debug = 0;

const CHIP_REG pps_time_cfg_reg[]=
{
	{PPS_TIME_CFG,TOP_BASE_ADDR,0x8,31,0},
};
const CHIP_REG phy_sel_cfg_reg[]=
{
	{PHY_SEL_CFG,TOP_BASE_ADDR,0x22,31,0},
};
const CHIP_REG clk_measure_cfg_reg[]=
{
	{MEASURE_TIME,TOP_BASE_ADDR,0x2a,1,0},
	{MEASURE_CLR,TOP_BASE_ADDR,0x2a,2,2},
};
const CHIP_REG cdr_sel_reg[]=
{
	{CDR_SEL,TOP_BASE_ADDR,0x46,1,0},
};
const CHIP_REG rst_glb_fuci_soft_n_reg[]=
{
	{RST_GLB_FUCI_SOFT_N,TOP_BASE_ADDR,0x0,0,0},
};
const CHIP_REG rst_glb_logic_n_reg[]=
{
	{RST_GLB_LOGIC_N,TOP_BASE_ADDR,0x1,0,0},
};
const CHIP_REG rst_logic_sys1_n_reg[]=
{
	{RST_FLEXE_MUX_N,TOP_BASE_ADDR,0x2,0,0},
	{RST_OAM_RX_N,TOP_BASE_ADDR,0x2,1,1},
	{RST_DEMUX_SYS_N,TOP_BASE_ADDR,0x2,3,3},
	{RST_CHANNELIZE_SYS_N,TOP_BASE_ADDR,0x2,4,4},
	{RST_B66SWITCH_SYS_N,TOP_BASE_ADDR,0x2,5,5},
	{RST_SOH_EXT_SCH_N,TOP_BASE_ADDR,0x2,6,6},
	{RST_SOH_INS_SCH_N,TOP_BASE_ADDR,0x2,7,7},
	{RST_SOAM_EXT_330M_0_N,TOP_BASE_ADDR,0x2,9,9},
	{RST_SOAM_EXT_330M_1_N,TOP_BASE_ADDR,0x2,10,10},
	{RST_SOAM_EXT_SCH_N,TOP_BASE_ADDR,0x2,11,11},
	{RST_SOAM_INS_DISTR_N,TOP_BASE_ADDR,0x2,12,12},
	{RST_SYS_DW_N,TOP_BASE_ADDR,0x2,13,13},
	{RST_SYS_UP_N,TOP_BASE_ADDR,0x2,14,14},
	{RST_SYS_TS_N,TOP_BASE_ADDR,0x2,15,15},
	{RST_FEC_ENC_N,TOP_BASE_ADDR,0x2,16,16},
	{RST_DEC_ENC_N,TOP_BASE_ADDR,0x2,17,17},
	{RST_SYS_CPBEGRESS_N,TOP_BASE_ADDR,0x2,18,18},
	{RST_CPB_INGRESS_N,TOP_BASE_ADDR,0x2,19,19},
	{RST_ENCODE_N,TOP_BASE_ADDR,0x2,20,20},
	{RST_DECODE_N,TOP_BASE_ADDR,0x2,21,21},
	{RST_RATEADP_N,TOP_BASE_ADDR,0x2,23,23},
	{RST_OAM_TX_N,TOP_BASE_ADDR,0x2,24,24},
	{RST_SYS_MCMACTX_N,TOP_BASE_ADDR,0x2,25,25},
	{RST_SYS_MCMACRX_N,TOP_BASE_ADDR,0x2,26,26},
	{RST_PPSTOD_N,TOP_BASE_ADDR,0x2,27,27},
	{RST_PCS_TX_ENV_N,TOP_BASE_ADDR,0x2,28,28},
	{RST_PCS_RX_SMOOTH_N,TOP_BASE_ADDR,0x2,29,29},
	{RST_FLEXE_MAC_N,TOP_BASE_ADDR,0x2,30,30},
	{RST_ILK_N,TOP_BASE_ADDR,0x2,31,31},
};
const CHIP_REG rst_fpga_serdes_n_reg[]=
{
	{RST_SERDES_100GE1_N,TOP_BASE_ADDR,0x3,0,0},
	{RST_SERDES_100GE2_N,TOP_BASE_ADDR,0x3,1,1},
	{RST_SERDES_50GE1_N,TOP_BASE_ADDR,0x3,2,2},
	{RST_SERDES_50GE2_N,TOP_BASE_ADDR,0x3,3,3},
	{RST_SERDES_50GE3_N,TOP_BASE_ADDR,0x3,4,4},
	{RST_SERDES_50GE4_N,TOP_BASE_ADDR,0x3,5,5},
	{RST_SERDES_GE_N,TOP_BASE_ADDR,0x3,6,6},
	{RST_SERDES_GE1_N,TOP_BASE_ADDR,0x3,7,7},
	{RST_SERDES_GE2_N,TOP_BASE_ADDR,0x3,8,8},
	{RST_SERDES_ILK_N,TOP_BASE_ADDR,0x3,9,9},
	{RST_SERDES_CFG_UPI_N,TOP_BASE_ADDR,0x3,10,10},
};
const CHIP_REG rst_logic_sys3_n_reg[]=
{
	{RST_OH_RX_0_N,TOP_BASE_ADDR,0x4,0,0},
	{RST_OH_RX_1_N,TOP_BASE_ADDR,0x4,1,1},
	{RST_OH_RX_2_N,TOP_BASE_ADDR,0x4,2,2},
	{RST_OH_RX_3_N,TOP_BASE_ADDR,0x4,3,3},
	{RST_OH_RX_4_N,TOP_BASE_ADDR,0x4,4,4},
	{RST_OH_RX_5_N,TOP_BASE_ADDR,0x4,5,5},
	{RST_OH_RX_6_N,TOP_BASE_ADDR,0x4,6,6},
	{RST_OH_RX_7_N,TOP_BASE_ADDR,0x4,7,7},
	{RST_OH_TX_0_N,TOP_BASE_ADDR,0x4,8,8},
	{RST_OH_TX_1_N,TOP_BASE_ADDR,0x4,9,9},
	{RST_OH_TX_2_N,TOP_BASE_ADDR,0x4,10,10},
	{RST_OH_TX_3_N,TOP_BASE_ADDR,0x4,11,11},
	{RST_OH_TX_4_N,TOP_BASE_ADDR,0x4,12,12},
	{RST_OH_TX_5_N,TOP_BASE_ADDR,0x4,13,13},
	{RST_OH_TX_6_N,TOP_BASE_ADDR,0x4,14,14},
	{RST_OH_TX_7_N,TOP_BASE_ADDR,0x4,15,15},
	{RST_DESKEW_0_N,TOP_BASE_ADDR,0x4,16,16},
	{RST_DESKEW_1_N,TOP_BASE_ADDR,0x4,17,17},
	{RST_DESKEW_2_N,TOP_BASE_ADDR,0x4,18,18},
	{RST_DESKEW_3_N,TOP_BASE_ADDR,0x4,19,19},
	{RST_DESKEW_4_N,TOP_BASE_ADDR,0x4,20,20},
	{RST_DESKEW_5_N,TOP_BASE_ADDR,0x4,21,21},
	{RST_DESKEW_6_N,TOP_BASE_ADDR,0x4,22,22},
	{RST_DESKEW_7_N,TOP_BASE_ADDR,0x4,23,23},
	{RST_SYS_HANDLING_N,TOP_BASE_ADDR,0x4,25,25},
	{RST_SYS_B66SARRX_N,TOP_BASE_ADDR,0x4,26,26},
	{RST_SYS_OAM_RX_N,TOP_BASE_ADDR,0x4,27,27},
	{RST_SYS_PORTSCH_DW_N,TOP_BASE_ADDR,0x4,28,28},
	{RST_SYS_OAM_TX_N,TOP_BASE_ADDR,0x4,29,29},
	{RST_SYS_RATEADP_N,TOP_BASE_ADDR,0x4,30,30},
	{RST_SYS_B66SARTX_N,TOP_BASE_ADDR,0x4,31,31},
};
const CHIP_REG rst_125m_pps_n_reg[]=
{
	{RST_125M_PPS_N,TOP_BASE_ADDR,0x5,0,0},
};
const CHIP_REG rst_fuci_module1_n_reg[]=
{
	{RST_UPI_FLEXE_MUX_N,TOP_BASE_ADDR,0x6,0,0},
	{RST_OAM_RX_UPI_N,TOP_BASE_ADDR,0x6,1,1},
	{RST_MR_PCS_RX_UPI_N,TOP_BASE_ADDR,0x6,2,2},
	{RST_DEMUX_UPI_SYS_N,TOP_BASE_ADDR,0x6,3,3},
	{RST_CHANNELIZE_UPI_SYS_N,TOP_BASE_ADDR,0x6,4,4},
	{RST_B66SWITCH_UPI_SYS_N,TOP_BASE_ADDR,0x6,5,5},
	{RST_SOH_UPI_DCI_N,TOP_BASE_ADDR,0x6,6,6},
	{RST_SYS_DW_UPI_N,TOP_BASE_ADDR,0x6,7,7},
	{RST_SYS_UP_UPI_N,TOP_BASE_ADDR,0x6,8,8},
	{RST_FEC_ENC_UPI_N,TOP_BASE_ADDR,0x6,9,9},
	{RST_FEC_DEC_UPI_N,TOP_BASE_ADDR,0x6,10,10},
	{RST_CPB_EGRESS_UPI_N,TOP_BASE_ADDR,0x6,11,11},
	{RST_CPB_INGRESS_UPI_N,TOP_BASE_ADDR,0x6,12,12},
	{RST_MR_PCS_TX_UPI_N,TOP_BASE_ADDR,0x6,13,13},
	{RST_RATEADP_UPI_N,TOP_BASE_ADDR,0x6,14,14},
	{RST_OAM_TX_UPI_N,TOP_BASE_ADDR,0x6,15,15},
	{RST_MACTX_UPI_N,TOP_BASE_ADDR,0x6,16,16},
	{RST_MACRX_UPI_N,TOP_BASE_ADDR,0x6,17,17},
	{RST_DESKEW_FUCI_0_N,TOP_BASE_ADDR,0x6,18,18},
	{RST_DESKEW_FUCI_1_N,TOP_BASE_ADDR,0x6,19,19},
	{RST_DESKEW_FUCI_2_N,TOP_BASE_ADDR,0x6,20,20},
	{RST_DESKEW_FUCI_3_N,TOP_BASE_ADDR,0x6,21,21},
	{RST_DESKEW_FUCI_4_N,TOP_BASE_ADDR,0x6,22,22},
	{RST_DESKEW_FUCI_5_N,TOP_BASE_ADDR,0x6,23,23},
	{RST_DESKEW_FUCI_6_N,TOP_BASE_ADDR,0x6,24,24},
	{RST_DESKEW_FUCI_7_N,TOP_BASE_ADDR,0x6,25,25},
	{RST_TX_ENV_GEARBOX_UPI_N,TOP_BASE_ADDR,0x6,26,26},
	{RST_RX_ENV_GEARBOX_UPI_N,TOP_BASE_ADDR,0x6,27,27},
	{RST_FLEXE_MAC_UPI_N,TOP_BASE_ADDR,0x6,28,28},
	{RST_FUCI_ILKN_N,TOP_BASE_ADDR,0x6,29,29},
};
const CHIP_REG rst_fuci_module2_n_reg[]=
{
	{RST_OH_RX_FUCI_0_N,TOP_BASE_ADDR,0x7,0,0},
	{RST_OH_RX_FUCI_1_N,TOP_BASE_ADDR,0x7,1,1},
	{RST_OH_RX_FUCI_2_N,TOP_BASE_ADDR,0x7,2,2},
	{RST_OH_RX_FUCI_3_N,TOP_BASE_ADDR,0x7,3,3},
	{RST_OH_RX_FUCI_4_N,TOP_BASE_ADDR,0x7,4,4},
	{RST_OH_RX_FUCI_5_N,TOP_BASE_ADDR,0x7,5,5},
	{RST_OH_RX_FUCI_6_N,TOP_BASE_ADDR,0x7,6,6},
	{RST_OH_RX_FUCI_7_N,TOP_BASE_ADDR,0x7,7,7},
	{RST_OH_TX_FUCI_0_N,TOP_BASE_ADDR,0x7,8,8},
	{RST_OH_TX_FUCI_1_N,TOP_BASE_ADDR,0x7,9,9},
	{RST_OH_TX_FUCI_2_N,TOP_BASE_ADDR,0x7,10,10},
	{RST_OH_TX_FUCI_3_N,TOP_BASE_ADDR,0x7,11,11},
	{RST_OH_TX_FUCI_4_N,TOP_BASE_ADDR,0x7,12,12},
	{RST_OH_TX_FUCI_5_N,TOP_BASE_ADDR,0x7,13,13},
	{RST_OH_TX_FUCI_6_N,TOP_BASE_ADDR,0x7,14,14},
	{RST_OH_TX_FUCI_7_N,TOP_BASE_ADDR,0x7,15,15},
	{RST_SYS_UPI_HANDLING_N,TOP_BASE_ADDR,0x7,17,17},
	{RST_SYS_UPI_CPBEGRESS_N,TOP_BASE_ADDR,0x7,18,18},
	{RST_SYS_UPI_MCMACTX_N,TOP_BASE_ADDR,0x7,19,19},
	{RST_SYS_UPI_B66SARRX_N,TOP_BASE_ADDR,0x7,20,20},
	{RST_SYS_UPI_OAM_RX_N,TOP_BASE_ADDR,0x7,21,21},
	{RST_SYS_UPI_OAM_TX_N,TOP_BASE_ADDR,0x7,22,22},
	{RST_SYS_UPI_RATEADP_N,TOP_BASE_ADDR,0x7,23,23},
	{RST_SYS_UPI_B66SARTX_N,TOP_BASE_ADDR,0x7,24,24},
	{RST_SYS_UPI_MCMACRX_N,TOP_BASE_ADDR,0x7,25,25},
	{RST_INF_CH_ADP_RX_UPI_N,TOP_BASE_ADDR,0x7,26,26},
	{RST_INF_CH_ADP_TX_UPI_N,TOP_BASE_ADDR,0x7,27,27},
	{RST_FLEXE_ENV_UPI_N,TOP_BASE_ADDR,0x7,28,28},
	{RST_REQ_GEN_UPI_N,TOP_BASE_ADDR,0x7,29,29},
};
const CHIP_REG rst_soh_ext_125m_n_reg[]=
{
	{RST_SOH_EXT_125M_N,TOP_BASE_ADDR,0x1d,0,0},
};
const CHIP_REG rst_soh_ins_125m_n_reg[]=
{
	{RST_SOH_INS_125M_N,TOP_BASE_ADDR,0x1e,0,0},
};
const CHIP_REG rst_soam_ext_125m_n_reg[]=
{
	{RST_SOAM_EXT_125M_N,TOP_BASE_ADDR,0x1f,0,0},
};
const CHIP_REG rst_soam_ins_125m_n_reg[]=
{
	{RST_SOAM_INS_125M_N,TOP_BASE_ADDR,0x20,0,0},
};
const CHIP_REG rst_gearbox_n_reg[]=
{
	{RST_GEARBOX_RX_N,TOP_BASE_ADDR,0x21,0,0},
	{RST_GEARBOX_TX_N,TOP_BASE_ADDR,0x21,1,1},
	{RST_CLK_MEASURE_N,TOP_BASE_ADDR,0x21,2,2},
	{RST_GEARBOX_RX_REQ_N,TOP_BASE_ADDR,0x21,3,3},
	{RST_GEARBOX_TX_REQ_N,TOP_BASE_ADDR,0x21,4,4},
};
const CHIP_REG rst_311m_n_reg[]=
{
	{RST_311M_B66SARRX_N,TOP_BASE_ADDR,0x23,0,0},
	{RST_311M_B66SARTX_N,TOP_BASE_ADDR,0x23,1,1},
	{RST_311M_TS_N,TOP_BASE_ADDR,0x23,2,2},
};
const CHIP_REG rst_311m_upi_n_reg[]=
{
	{RST_311M_UPI_B66SARTX_N,TOP_BASE_ADDR,0x24,0,0},
	{RST_311M_UPI_B66SARRX_N,TOP_BASE_ADDR,0x24,1,1},
};
const CHIP_REG rst_soh_upi_125ext_n_reg[]=
{
	{RST_SOH_UPI_125EXT_N,TOP_BASE_ADDR,0x25,0,0},
};
const CHIP_REG rst_soh_upi_125ins_n_reg[]=
{
	{RST_SOH_UPI_125INS_N,TOP_BASE_ADDR,0x26,0,0},
};
const CHIP_REG rst_soam_upi_125ext_n_reg[]=
{
	{RST_SOAM_UPI_125EXT_N,TOP_BASE_ADDR,0x27,0,0},
};
const CHIP_REG rst_soam_upi_125ins_n_reg[]=
{
	{RST_SOAM_UPI_125INS_N,TOP_BASE_ADDR,0x28,0,0},
};
const CHIP_REG rst_soh_upi_fuci_n_reg[]=
{
	{RST_SOH_UPI_FUCI_N,TOP_BASE_ADDR,0x29,0,0},
	{RST_SUBSYS_MAC_FUCI_N,TOP_BASE_ADDR,0x29,1,1},
};
const CHIP_REG rst_sys_logic4_n_reg[]=
{
	{RST_SYS_PORTSCH_UP_N,TOP_BASE_ADDR,0x45,0,0},
	{RST_INF_CH_ADP_RX_N,TOP_BASE_ADDR,0x45,1,1},
	{RST_INF_CH_ADP_TX_N,TOP_BASE_ADDR,0x45,2,2},
	{RST_FLEXE_ENV_N,TOP_BASE_ADDR,0x45,3,3},
	{RST_REQ_GEN_N,TOP_BASE_ADDR,0x45,4,4},
	{RST_MR_PCS_RX_0_N,TOP_BASE_ADDR,0x45,5,5},
	{RST_MR_PCS_RX_1_N,TOP_BASE_ADDR,0x45,6,6},
	{RST_MR_PCS_RX_2_N,TOP_BASE_ADDR,0x45,7,7},
	{RST_MR_PCS_RX_3_N,TOP_BASE_ADDR,0x45,8,8},
	{RST_MR_PCS_TX_0_N,TOP_BASE_ADDR,0x45,9,9},
	{RST_MR_PCS_TX_1_N,TOP_BASE_ADDR,0x45,10,10},
	{RST_MR_PCS_TX_2_N,TOP_BASE_ADDR,0x45,11,11},
	{RST_MR_PCS_TX_3_N,TOP_BASE_ADDR,0x45,12,12},
	{RST_FEC_DEC_0_N,TOP_BASE_ADDR,0x45,13,13},
	{RST_FEC_DEC_1_N,TOP_BASE_ADDR,0x45,14,14},
	{RST_FEC_ENC_0_N,TOP_BASE_ADDR,0x45,15,15},
	{RST_FEC_ENC_1_N,TOP_BASE_ADDR,0x45,16,16},
	{RST_OH_N,TOP_BASE_ADDR,0x45,17,17},
	{RST_OHIF_1588_RATE_LIMIT_N,TOP_BASE_ADDR,0x45,18,18},
};
const CHIP_REG rst_ieee1588_1_reg[]=
{
	{RST_FLEXE_PTP_RX_0_N,TOP_BASE_ADDR,0x47,0,0},
	{RST_FLEXE_PTP_RX_1_N,TOP_BASE_ADDR,0x47,1,1},
	{RST_FLEXE_PTP_RX_2_N,TOP_BASE_ADDR,0x47,2,2},
	{RST_FLEXE_PTP_RX_3_N,TOP_BASE_ADDR,0x47,3,3},
	{RST_FLEXE_PTP_RX_4_N,TOP_BASE_ADDR,0x47,4,4},
	{RST_FLEXE_PTP_RX_5_N,TOP_BASE_ADDR,0x47,5,5},
	{RST_FLEXE_PTP_RX_6_N,TOP_BASE_ADDR,0x47,6,6},
	{RST_FLEXE_PTP_RX_7_N,TOP_BASE_ADDR,0x47,7,7},
	{RST_FLEXE_PTP_RX_8_N,TOP_BASE_ADDR,0x47,8,8},
	{RST_FLEXE_PTP_RX_9_N,TOP_BASE_ADDR,0x47,9,9},
	{RST_FLEXE_PTP_RX_10_N,TOP_BASE_ADDR,0x47,10,10},
	{RST_FLEXE_PTP_RX_11_N,TOP_BASE_ADDR,0x47,11,11},
	{RST_FLEXE_PTP_RX_12_N,TOP_BASE_ADDR,0x47,12,12},
	{RST_FLEXE_PTP_RX_13_N,TOP_BASE_ADDR,0x47,13,13},
	{RST_FLEXE_PTP_RX_14_N,TOP_BASE_ADDR,0x47,14,14},
	{RST_FLEXE_PTP_RX_15_N,TOP_BASE_ADDR,0x47,15,15},
	{RST_FLEXE_PTP_TX_0_N,TOP_BASE_ADDR,0x47,16,16},
	{RST_FLEXE_PTP_TX_1_N,TOP_BASE_ADDR,0x47,17,17},
	{RST_FLEXE_PTP_TX_2_N,TOP_BASE_ADDR,0x47,18,18},
	{RST_FLEXE_PTP_TX_3_N,TOP_BASE_ADDR,0x47,19,19},
	{RST_FLEXE_PTP_TX_4_N,TOP_BASE_ADDR,0x47,20,20},
	{RST_FLEXE_PTP_TX_5_N,TOP_BASE_ADDR,0x47,21,21},
	{RST_FLEXE_PTP_TX_6_N,TOP_BASE_ADDR,0x47,22,22},
	{RST_FLEXE_PTP_TX_7_N,TOP_BASE_ADDR,0x47,23,23},
	{RST_XGMII_RX_N,TOP_BASE_ADDR,0x47,24,24},
	{RST_XGMII_TX_N,TOP_BASE_ADDR,0x47,25,25},
	{RST_CPU_RX_N,TOP_BASE_ADDR,0x47,26,26},
	{RST_CPU_TX_N,TOP_BASE_ADDR,0x47,27,27},
};
const CHIP_REG rst_ieee1588_2_reg[]=
{
	{RST_UPI_FLEXE_PTP_RX_0_N,TOP_BASE_ADDR,0x48,0,0},
	{RST_UPI_FLEXE_PTP_RX_1_N,TOP_BASE_ADDR,0x48,1,1},
	{RST_UPI_FLEXE_PTP_RX_2_N,TOP_BASE_ADDR,0x48,2,2},
	{RST_UPI_FLEXE_PTP_RX_3_N,TOP_BASE_ADDR,0x48,3,3},
	{RST_UPI_FLEXE_PTP_RX_4_N,TOP_BASE_ADDR,0x48,4,4},
	{RST_UPI_FLEXE_PTP_RX_5_N,TOP_BASE_ADDR,0x48,5,5},
	{RST_UPI_FLEXE_PTP_RX_6_N,TOP_BASE_ADDR,0x48,6,6},
	{RST_UPI_FLEXE_PTP_RX_7_N,TOP_BASE_ADDR,0x48,7,7},
	{RST_UPI_FLEXE_PTP_RX_8_N,TOP_BASE_ADDR,0x48,8,8},
	{RST_UPI_FLEXE_PTP_RX_9_N,TOP_BASE_ADDR,0x48,9,9},
	{RST_UPI_FLEXE_PTP_RX_10_N,TOP_BASE_ADDR,0x48,10,10},
	{RST_UPI_FLEXE_PTP_RX_11_N,TOP_BASE_ADDR,0x48,11,11},
	{RST_UPI_FLEXE_PTP_RX_12_N,TOP_BASE_ADDR,0x48,12,12},
	{RST_UPI_FLEXE_PTP_RX_13_N,TOP_BASE_ADDR,0x48,13,13},
	{RST_UPI_FLEXE_PTP_RX_14_N,TOP_BASE_ADDR,0x48,14,14},
	{RST_UPI_FLEXE_PTP_RX_15_N,TOP_BASE_ADDR,0x48,15,15},
	{RST_UPI_FLEXE_PTP_TX_0_N,TOP_BASE_ADDR,0x48,16,16},
	{RST_UPI_FLEXE_PTP_TX_1_N,TOP_BASE_ADDR,0x48,17,17},
	{RST_UPI_FLEXE_PTP_TX_2_N,TOP_BASE_ADDR,0x48,18,18},
	{RST_UPI_FLEXE_PTP_TX_3_N,TOP_BASE_ADDR,0x48,19,19},
	{RST_UPI_FLEXE_PTP_TX_4_N,TOP_BASE_ADDR,0x48,20,20},
	{RST_UPI_FLEXE_PTP_TX_5_N,TOP_BASE_ADDR,0x48,21,21},
	{RST_UPI_FLEXE_PTP_TX_6_N,TOP_BASE_ADDR,0x48,22,22},
	{RST_UPI_FLEXE_PTP_TX_7_N,TOP_BASE_ADDR,0x48,23,23},
	{RST_UPI_GLOBAL_IEEE1588_N,TOP_BASE_ADDR,0x48,24,24},
	{RST_UPI_XGMII_RX_N,TOP_BASE_ADDR,0x48,25,25},
	{RST_UPI_XGMII_TX_N,TOP_BASE_ADDR,0x48,26,26},
	{RST_UPI_CCU_GE_N,TOP_BASE_ADDR,0x48,27,27},
	{RST_UPI_CPU_N,TOP_BASE_ADDR,0x48,28,28},
};
const CHIP_REG glb_clr_reg[]=
{
	{GLB_CNT_CLR,TOP_BASE_ADDR,0xf,0,0},
	{GLB_ALM_CLR,TOP_BASE_ADDR,0xf,1,1},
	{GLB_INT_CLR,TOP_BASE_ADDR,0xf,2,2},
};
const CHIP_REG pps_tod_rx_num_reg[]=
{
	{PPS_TOD_RX_NUM,TOP_BASE_ADDR,0x9,15,0},
};
const CHIP_REG pps_tod_rx_type_reg[]=
{
	{PPS_TOD_RX_TYPE,TOP_BASE_ADDR,0xa,15,0},
};
const CHIP_REG pps_tod_rx_len_reg[]=
{
	{PPS_TOD_RX_LEN,TOP_BASE_ADDR,0xb,15,0},
};
const CHIP_REG pps_tod_rx_tod_byte0_reg[]=
{
	{PPS_TOD_RX_TOD_BYTE0,TOP_BASE_ADDR,0xc,7,0},
};
const CHIP_REG pps_tod_rx_tod_sec_reg[]=
{
	{PPS_TOD_RX_TOD_SEC_0,TOP_BASE_ADDR,0xd,31,0},
	{PPS_TOD_RX_TOD_SEC_1,TOP_BASE_ADDR,0xe,15,0},
};
const CHIP_REG pps_tod_rx_tod_nsec_reg[]=
{
	{PPS_TOD_RX_TOD_NSEC,TOP_BASE_ADDR,0x10,31,0},
};
const CHIP_REG pps_tod_rx_tod_res_reg[]=
{
	{PPS_TOD_RX_TOD_RES,TOP_BASE_ADDR,0x11,31,0},
};
const CHIP_REG pps_tod_rx_crc_reg[]=
{
	{PPS_TOD_RX_CRC,TOP_BASE_ADDR,0x12,7,0},
};
const CHIP_REG pps_tod_rx_real_time_reg[]=
{
	{PPS_TOD_RX_REAL_TIME_0,TOP_BASE_ADDR,0x13,31,0},
	{PPS_TOD_RX_REAL_TIME_1,TOP_BASE_ADDR,0x14,31,0},
	{PPS_TOD_RX_REAL_TIME_2,TOP_BASE_ADDR,0x15,15,0},
};
const CHIP_REG pps_tod_rx_cnt_tod_lost_reg[]=
{
	{PPS_TOD_RX_CNT_TOD_LOST,TOP_BASE_ADDR,0x16,15,0},
};
const CHIP_REG pps_tod_rx_cnt_pps_adjust_reg[]=
{
	{PPS_TOD_RX_CNT_PPS_ADJUST,TOP_BASE_ADDR,0x17,15,0},
};
const CHIP_REG pps_tod_rx_cnt_crc_eor_reg[]=
{
	{PPS_TOD_RX_CNT_CRC_EOR,TOP_BASE_ADDR,0x18,15,0},
};
const CHIP_REG pps_tod_rx_cnt_wave_broken_reg[]=
{
	{PPS_TOD_RX_CNT_WAVE_BROKEN,TOP_BASE_ADDR,0x19,15,0},
};
const CHIP_REG pps_tod_rx_reg_cnt_25m_reg[]=
{
	{PPS_TOD_RX_REG_CNT_25M,TOP_BASE_ADDR,0x1a,2,0},
};
const CHIP_REG measure_cnt_max_reg[]=
{
	{MEASURE_CNT_MAX,TOP_BASE_ADDR,0x2b,29,0},
};
const CHIP_REG measure_cnt_min_reg[]=
{
	{MEASURE_CNT_MIN,TOP_BASE_ADDR,0x38,29,0},
};
const CHIP_REG top_int1_INT_REG[]=
{
	{OH_RX0_INT,TOP_BASE_ADDR,0x51,0,0},
	{OH_RX1_INT,TOP_BASE_ADDR,0x51,1,1},
	{OH_RX2_INT,TOP_BASE_ADDR,0x51,2,2},
	{OH_RX3_INT,TOP_BASE_ADDR,0x51,3,3},
	{OH_RX4_INT,TOP_BASE_ADDR,0x51,4,4},
	{OH_RX5_INT,TOP_BASE_ADDR,0x51,5,5},
	{OH_RX6_INT,TOP_BASE_ADDR,0x51,6,6},
	{OH_RX7_INT,TOP_BASE_ADDR,0x51,7,7},
	{FLEXE_OAM_RX0_INT,TOP_BASE_ADDR,0x51,8,8},
	{FLEXE_OAM_RX2_INT,TOP_BASE_ADDR,0x51,10,10},
	{FLEXE_OAM_RX3_INT,TOP_BASE_ADDR,0x51,11,11},
	{FLEXE_OAM_RX4_INT,TOP_BASE_ADDR,0x51,12,12},
	{FLEXE_OAM_RX5_INT,TOP_BASE_ADDR,0x51,13,13},
	{FLEXE_OAM_RX6_INT,TOP_BASE_ADDR,0x51,14,14},
	{FLEXE_OAM_RX7_INT,TOP_BASE_ADDR,0x51,15,15},
	{FLEXE_OAM_RX8_INT,TOP_BASE_ADDR,0x51,16,16},
	{FLEXE_OAM_RX9_INT,TOP_BASE_ADDR,0x51,17,17},
	{FLEXE_OAM_RX10_INT,TOP_BASE_ADDR,0x51,18,18},
	{FLEXE_OAM_RX11_INT,TOP_BASE_ADDR,0x51,19,19},
	{FLEXE_OAM_TX0_INT,TOP_BASE_ADDR,0x51,20,20},
	{FLEXE_OAM_TX1_INT,TOP_BASE_ADDR,0x51,21,21},
	{FLEXE_OAM_TX2_INT,TOP_BASE_ADDR,0x51,22,22},
	{SAR_RX_INT,TOP_BASE_ADDR,0x51,23,23},
	{SAR_TX_INT,TOP_BASE_ADDR,0x51,24,24},
	{SAR_OAM_RX0_INT,TOP_BASE_ADDR,0x51,25,25},
	{SAR_OAM_RX2_INT,TOP_BASE_ADDR,0x51,27,27},
	{SAR_OAM_RX3_INT,TOP_BASE_ADDR,0x51,28,28},
	{SAR_OAM_RX4_INT,TOP_BASE_ADDR,0x51,29,29},
	{SAR_OAM_RX5_INT,TOP_BASE_ADDR,0x51,30,30},
	{SAR_OAM_RX6_INT,TOP_BASE_ADDR,0x51,31,31},
};
const CHIP_REG top_int1_INT_MASK_REG[]=
{
	{OH_RX0_INT_MASK,TOP_BASE_ADDR,0x52,0,0},
	{OH_RX1_INT_MASK,TOP_BASE_ADDR,0x52,1,1},
	{OH_RX2_INT_MASK,TOP_BASE_ADDR,0x52,2,2},
	{OH_RX3_INT_MASK,TOP_BASE_ADDR,0x52,3,3},
	{OH_RX4_INT_MASK,TOP_BASE_ADDR,0x52,4,4},
	{OH_RX5_INT_MASK,TOP_BASE_ADDR,0x52,5,5},
	{OH_RX6_INT_MASK,TOP_BASE_ADDR,0x52,6,6},
	{OH_RX7_INT_MASK,TOP_BASE_ADDR,0x52,7,7},
	{FLEXE_OAM_RX0_INT_MASK,TOP_BASE_ADDR,0x52,8,8},
	{FLEXE_OAM_RX2_INT_MASK,TOP_BASE_ADDR,0x52,10,10},
	{FLEXE_OAM_RX3_INT_MASK,TOP_BASE_ADDR,0x52,11,11},
	{FLEXE_OAM_RX4_INT_MASK,TOP_BASE_ADDR,0x52,12,12},
	{FLEXE_OAM_RX5_INT_MASK,TOP_BASE_ADDR,0x52,13,13},
	{FLEXE_OAM_RX6_INT_MASK,TOP_BASE_ADDR,0x52,14,14},
	{FLEXE_OAM_RX7_INT_MASK,TOP_BASE_ADDR,0x52,15,15},
	{FLEXE_OAM_RX8_INT_MASK,TOP_BASE_ADDR,0x52,16,16},
	{FLEXE_OAM_RX9_INT_MASK,TOP_BASE_ADDR,0x52,17,17},
	{FLEXE_OAM_RX10_INT_MASK,TOP_BASE_ADDR,0x52,18,18},
	{FLEXE_OAM_RX11_INT_MASK,TOP_BASE_ADDR,0x52,19,19},
	{FLEXE_OAM_TX0_INT_MASK,TOP_BASE_ADDR,0x52,20,20},
	{FLEXE_OAM_TX1_INT_MASK,TOP_BASE_ADDR,0x52,21,21},
	{FLEXE_OAM_TX2_INT_MASK,TOP_BASE_ADDR,0x52,22,22},
	{SAR_RX_INT_MASK,TOP_BASE_ADDR,0x52,23,23},
	{SAR_TX_INT_MASK,TOP_BASE_ADDR,0x52,24,24},
	{SAR_OAM_RX0_INT_MASK,TOP_BASE_ADDR,0x52,25,25},
	{SAR_OAM_RX2_INT_MASK,TOP_BASE_ADDR,0x52,27,27},
	{SAR_OAM_RX3_INT_MASK,TOP_BASE_ADDR,0x52,28,28},
	{SAR_OAM_RX4_INT_MASK,TOP_BASE_ADDR,0x52,29,29},
	{SAR_OAM_RX5_INT_MASK,TOP_BASE_ADDR,0x52,30,30},
	{SAR_OAM_RX6_INT_MASK,TOP_BASE_ADDR,0x52,31,31},
};
const CHIP_REG top_int2_INT_REG[]=
{
	{SAR_OAM_RX7_INT,TOP_BASE_ADDR,0x53,1,1},
	{SAR_OAM_RX8_INT,TOP_BASE_ADDR,0x53,2,2},
	{SAR_OAM_RX9_INT,TOP_BASE_ADDR,0x53,3,3},
	{SAR_OAM_RX10_INT,TOP_BASE_ADDR,0x53,4,4},
	{SAR_OAM_RX11_INT,TOP_BASE_ADDR,0x53,5,5},
	{SAR_OAM_TX0_INT,TOP_BASE_ADDR,0x53,6,6},
	{SAR_OAM_TX1_INT,TOP_BASE_ADDR,0x53,7,7},
	{SAR_OAM_TX2_INT,TOP_BASE_ADDR,0x53,8,8},
	{IEEE_1588_INT,TOP_BASE_ADDR,0x53,9,9},
	{SAR_OAM_RX13_INT,TOP_BASE_ADDR,0x53,12,12},
	{FLEXE_OAM_RX13_INT,TOP_BASE_ADDR,0x53,13,13},
	{SAR_OAM_RX14_INT,TOP_BASE_ADDR,0x53,14,14},
	{FLEXE_OAM_RX14_INT,TOP_BASE_ADDR,0x53,15,15},
	{SAR_OAM_RX15_INT,TOP_BASE_ADDR,0x53,16,16},
	{FLEXE_OAM_RX15_INT,TOP_BASE_ADDR,0x53,17,17},
	{PORT_BUFFER0_INT,TOP_BASE_ADDR,0x53,18,18},
	{PORT_BUFFER1_INT,TOP_BASE_ADDR,0x53,19,19},
	{PORT_BUFFER2_INT,TOP_BASE_ADDR,0x53,20,20},
	{PORT_BUFFER3_INT,TOP_BASE_ADDR,0x53,21,21},
	{PORT_BUFFER4_INT,TOP_BASE_ADDR,0x53,22,22},
	{PORT_BUFFER5_INT,TOP_BASE_ADDR,0x53,23,23},
	{PORT_BUFFER6_INT,TOP_BASE_ADDR,0x53,24,24},
	{PORT_BUFFER7_INT,TOP_BASE_ADDR,0x53,25,25},
};
const CHIP_REG top_int2_INT_MASK_REG[]=
{
	{SAR_OAM_RX7_INT_MASK,TOP_BASE_ADDR,0x54,1,1},
	{SAR_OAM_RX8_INT_MASK,TOP_BASE_ADDR,0x54,2,2},
	{SAR_OAM_RX9_INT_MASK,TOP_BASE_ADDR,0x54,3,3},
	{SAR_OAM_RX10_INT_MASK,TOP_BASE_ADDR,0x54,4,4},
	{SAR_OAM_RX11_INT_MASK,TOP_BASE_ADDR,0x54,5,5},
	{SAR_OAM_TX0_INT_MASK,TOP_BASE_ADDR,0x54,6,6},
	{SAR_OAM_TX1_INT_MASK,TOP_BASE_ADDR,0x54,7,7},
	{SAR_OAM_TX2_INT_MASK,TOP_BASE_ADDR,0x54,8,8},
	{IEEE_1588_INT_MASK,TOP_BASE_ADDR,0x54,9,9},
	{SAR_OAM_RX13_INT_MASK,TOP_BASE_ADDR,0x54,12,12},
	{FLEXE_OAM_RX13_INT_MASK,TOP_BASE_ADDR,0x54,13,13},
	{SAR_OAM_RX14_INT_MASK,TOP_BASE_ADDR,0x54,14,14},
	{FLEXE_OAM_RX14_INT_MASK,TOP_BASE_ADDR,0x54,15,15},
	{SAR_OAM_RX15_INT_MASK,TOP_BASE_ADDR,0x54,16,16},
	{FLEXE_OAM_RX15_INT_MASK,TOP_BASE_ADDR,0x54,17,17},
	{PORT_BUFFER0_INT_MASK,TOP_BASE_ADDR,0x54,18,18},
	{PORT_BUFFER1_INT_MASK,TOP_BASE_ADDR,0x54,19,19},
	{PORT_BUFFER2_INT_MASK,TOP_BASE_ADDR,0x54,20,20},
	{PORT_BUFFER3_INT_MASK,TOP_BASE_ADDR,0x54,21,21},
	{PORT_BUFFER4_INT_MASK,TOP_BASE_ADDR,0x54,22,22},
	{PORT_BUFFER5_INT_MASK,TOP_BASE_ADDR,0x54,23,23},
	{PORT_BUFFER6_INT_MASK,TOP_BASE_ADDR,0x54,24,24},
	{PORT_BUFFER7_INT_MASK,TOP_BASE_ADDR,0x54,25,25},
};


/******************************************************************************
*
* FUNCTION
*
*     top_rst_glb_fuci_soft_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_glb_fuci_soft_n_reset(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_glb_fuci_soft_n_reg[RST_GLB_FUCI_SOFT_N],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_glb_logic_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_glb_logic_n_reset(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_glb_logic_n_reg[RST_GLB_LOGIC_N],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_logic_sys1_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:0~28
*			   0:RST_FLEXE_MUX_N  1:RST_OAM_RX_N  2:RST_DEMUX_SYS_N  3:RST_CHANNELIZE_SYS_N 
*			   4:RST_B66SWITCH_SYS_N 5:RST_SOH_EXT_SCH_N  6:RST_SOH_INS_SCH_N  7:RST_SOAM_EXT_330M_0_N
*			   8:RST_SOAM_EXT_330M_1_N   9:RST_SOAM_EXT_SCH_N   10:RST_SOAM_INS_DISTR_N     
*			   11:RST_SYS_DW_N  12:RST_SYS_UP_N  13:RST_SYS_TS_N    14:RST_FEC_ENC_N 15:RST_DEC_ENC_N  
*			   16:RST_SYS_CPBEGRESS_N  17:RST_CPB_INGRESS_N  18:RST_ENCODE_N  19:RST_DECODE_N  
*			   20:RST_RATEADP_N  21:RST_OAM_TX_N  22:RST_SYS_MCMACTX_N  23:RST_SYS_MCMACRX_N  
*			   24:RST_PPSTOD_N  25:RST_PCS_TX_ENV_N  26:RST_PCS_RX_SMOOTH_N  27:RST_FLEXE_MAC_N
*			   28:RST_ILK_N
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_logic_sys1_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_logic_sys1_n_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_logic_sys1_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_fpga_serdes_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:0~10
*			   0:100GEserdes-1  1:100GEserdes-2  2:50GEserdes-1  3:50GEserdes-2
*			   4:50GEserdes-3   5:50GEserdes-4   6:GEserdes      7:GEserdes-1
*			   8:GEserdes-2     9:ilk-serdes     10:upi-serdes
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_fpga_serdes_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_fpga_serdes_n_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_fpga_serdes_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_logic_sys3_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:0~30
*			   0:oh-rx-0  1:oh-rx-1  2:oh-rx-2  3:oh-rx-3  4:oh-rx-4  5:oh-rx-5
*			   6:oh-rx-6  7:oh-rx-7  8:oh-tx-0  9:oh-tx-1  10:oh-tx-2  11:oh-tx-3
*			   12:oh-tx-4  13:oh-tx-5  14:oh-tx-6  15:oh-tx-7  16:flexe-deskew-0
*			   17:flexe-deskew-1  18:flexe-deskew-2  19:flexe-deskew-3  20:flexe-deskew-4
*			   21:flexe-deskew-5  22:flexe-deskew-6  23:flexe-deskew-7  24:RST_SYS_HANDLING_N
*			   25:RST_SYS_B66SARRX_N  26:RST_SYS_OAM_RX_N  27:RST_SYS_PORTSCH_DW_N
*			   28:RST_SYS_OAM_TX_N  29:RST_SYS_RATEADP_N  30:RST_SYS_B66SARTX_N
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_logic_sys3_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_logic_sys3_n_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_logic_sys3_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_125m_pps_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_125m_pps_n_reset(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_125m_pps_n_reg[RST_125M_PPS_N],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_fuci_module1_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:0~25
*			   0:flexe mux-upi  1:oam_rx-upi  2:mr_pcs_rx-upi  3:demux-fuci  4:channelize-fuci
*			   5:b66switch-fuci  6:fuci-fuci  7:sar_rx fuci  8:sar_tx fuci  9:fec coding fuci
*			   10:fec decoding fuci  11:cpb_egress fuci  12:cpb_ingress fuci  13:mr_pcs_tx upi
*			   14:speed upi  15:oam_tx upi  16:mactx upi  17:macrx upi  18:flexe deskew fuci 0
*			   19:flexe deskew fuci 1  20:flexe deskew fuci 2  21:flexe deskew fuci 3
*			   22:flexe deskew fuci 4  23:flexe deskew fuci 5  24:flexe deskew fuci 6
*			   25:flexe deskew fuci 7
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_fuci_module1_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_fuci_module1_n_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_fuci_module1_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_fuci_module2_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:0~28
*			   0:oh-rx-0  1:oh-rx-1  2:oh-rx-2  3:oh-rx-3  4:oh-rx-4  5:oh-rx-5
*			   6:oh-rx-6  7:oh-rx-7  8:oh-tx-0  9:oh-tx-1  10:oh-tx-2  11:oh-tx-3
*			   12:oh-tx-4  13:oh-tx-5  14:oh-tx-6  15:oh-tx-7  
*			   16:RST_SYS_UPI_HANDLING_N  17:RST_SYS_UPI_CPBEGRESS_N  18:RST_SYS_UPI_MCMACTX_N
*			   19:RST_SYS_UPI_B66SARRX_N  20:RST_SYS_UPI_OAM_RX_N  21:RST_SYS_UPI_OAM_TX_N
*			   22:RST_SYS_UPI_RATEADP_N   23:RST_SYS_UPI_B66SARTX_N  24:RST_SYS_UPI_MCMACRX_N
*			   25:RST_INF_CH_ADP_RX_UPI_N  26:RST_INF_CH_ADP_TX_UPI_N  27:RST_FLEXE_ENV_UPI_N
*			   28:RST_REQ_GEN_UPI_N
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_fuci_module2_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_fuci_module2_n_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_fuci_module2_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_pps_time_cfg_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_pps_time_cfg_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0xffffffff))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,pps_time_cfg_reg[PPS_TIME_CFG],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_pps_time_cfg_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_pps_time_cfg_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,pps_time_cfg_reg[PPS_TIME_CFG],parameter);
	
	return RET_SUCCESS;
}

RET_STATUS top_pps_tod_rx_num_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,pps_tod_rx_num_reg[PPS_TOD_RX_NUM],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_pps_tod_rx_type_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_pps_tod_rx_type_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,pps_tod_rx_type_reg[PPS_TOD_RX_TYPE],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_pps_tod_rx_len_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_pps_tod_rx_len_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,pps_tod_rx_len_reg[PPS_TOD_RX_LEN],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_pps_tod_rx_tod_byte0_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_pps_tod_rx_tod_byte0_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,pps_tod_rx_tod_byte0_reg[PPS_TOD_RX_TOD_BYTE0],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_pps_tod_rx_tod_sec_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: field select 0~1
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_pps_tod_rx_tod_sec_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!!\r\nchip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(pps_tod_rx_tod_sec_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,pps_tod_rx_tod_sec_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_glb_cnt_clr_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_glb_cnt_clr_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	parameter = parameter & MASK_1_BIT;

	regp_write(chip_id,glb_clr_reg[GLB_CNT_CLR].base_addr,glb_clr_reg[GLB_CNT_CLR].offset_addr,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_glb_alm_clr_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_glb_alm_clr_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (GLB_ALM_CLR > MAX_INDEX(glb_clr_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	parameter = (parameter & MASK_1_BIT) << SHIFT_BIT_1;

	regp_write(chip_id,glb_clr_reg[GLB_ALM_CLR].base_addr,glb_clr_reg[GLB_ALM_CLR].offset_addr,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_glb_int_clr_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS top_glb_int_clr_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (GLB_INT_CLR > MAX_INDEX(glb_clr_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	parameter = (parameter & MASK_1_BIT) << SHIFT_BIT_2;

	regp_write(chip_id,glb_clr_reg[GLB_INT_CLR].base_addr,glb_clr_reg[GLB_INT_CLR].offset_addr,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_pps_tod_rx_tod_nsec_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_pps_tod_rx_tod_nsec_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,pps_tod_rx_tod_nsec_reg[PPS_TOD_RX_TOD_NSEC],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_pps_tod_rx_tod_res_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_pps_tod_rx_tod_res_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,pps_tod_rx_tod_res_reg[PPS_TOD_RX_TOD_RES],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_pps_tod_rx_crc_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_pps_tod_rx_crc_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,pps_tod_rx_crc_reg[PPS_TOD_RX_CRC],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_pps_tod_rx_real_time_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:field select 0~2
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_pps_tod_rx_real_time_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!!\r\n chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(pps_tod_rx_real_time_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,pps_tod_rx_real_time_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_pps_tod_rx_cnt_tod_lost_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_pps_tod_rx_cnt_tod_lost_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,pps_tod_rx_cnt_tod_lost_reg[PPS_TOD_RX_CNT_TOD_LOST],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_pps_tod_rx_cnt_pps_adjust_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_pps_tod_rx_cnt_pps_adjust_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,pps_tod_rx_cnt_pps_adjust_reg[PPS_TOD_RX_CNT_PPS_ADJUST],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_pps_tod_rx_cnt_crc_eor_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_pps_tod_rx_cnt_crc_eor_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,pps_tod_rx_cnt_crc_eor_reg[PPS_TOD_RX_CNT_CRC_EOR],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_pps_tod_rx_cnt_wave_broken_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_pps_tod_rx_cnt_wave_broken_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,pps_tod_rx_cnt_wave_broken_reg[PPS_TOD_RX_CNT_WAVE_BROKEN],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_pps_tod_rx_reg_cnt_25m_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_pps_tod_rx_reg_cnt_25m_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,pps_tod_rx_reg_cnt_25m_reg[PPS_TOD_RX_REG_CNT_25M],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     top_rst_soh_ext_125m_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:field select 0~1  0:125m  1:clk_soh_ext_125m fuci
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_soh_ext_125m_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_soh_ext_125m_n_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_soh_ext_125m_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_soh_ins_125m_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:field select 0~1 0:125m  1:clk_soh_ins_125m fuci
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_soh_ins_125m_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_soh_ins_125m_n_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_soh_ins_125m_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_soam_ext_125m_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:field select 0~1 0:OAM 125m  1:clk_soam_ext_125m fuci
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_soam_ext_125m_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_soam_ext_125m_n_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_soam_ext_125m_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_soam_ins_125m_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:field select 0~1 0:OAM 125m  1:clk_soam_ins_125m fuci
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_soam_ins_125m_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_soam_ins_125m_n_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_soam_ins_125m_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_gearbox_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:field select 0~1 0:gearbox_rx upi  1:gearbox_tx upi
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_gearbox_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_gearbox_n_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_gearbox_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_psy_sel_cfg_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  channel_id: channel select 0~7
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-23    1.0           initial
*
******************************************************************************/
RET_STATUS top_psy_sel_cfg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > (PHY_NUM-1)) || (parameter > 0xf))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n channel_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  channel_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_bit_write(chip_id,phy_sel_cfg_reg[PHY_SEL_CFG].base_addr,phy_sel_cfg_reg[PHY_SEL_CFG].offset_addr,channel_id*4+3,channel_id*4,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_psy_sel_cfg_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  channel_id: channel select 0~7
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-23    1.0           initial
*
******************************************************************************/
RET_STATUS top_psy_sel_cfg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > (PHY_NUM-1)))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n channel_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  channel_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,phy_sel_cfg_reg[PHY_SEL_CFG].base_addr,phy_sel_cfg_reg[PHY_SEL_CFG].offset_addr,channel_id*4+3,channel_id*4,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_311m_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: field select 0~2  0:RST_311M_B66SARRX_N  1:RST_311M_B66SARTX_N  2:RST_311M_TS_N
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-23    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_311m_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_311m_n_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_311m_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_311m_upi_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: field select 0~2  0:RST_311M_UPI_B66SARTX_N  1:RST_311M_UPI_B66SARRX_N
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-23    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_311m_upi_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_311m_upi_n_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_311m_upi_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     top_soh_upi_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: 0:RST_SOH_UPI_125EXT_N
*	  1:RST_SOH_UPI_125INS_N
*	  2:RST_SOAM_UPI_125EXT_N
*	  3:RST_SOAM_UPI_125INS_N
*	  4:RST_SOH_UPI_FUCI_N
*	  5:RST_SUBSYS_MAC_FUCI_N
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-1-23    1.0           initial
*
******************************************************************************/
RET_STATUS top_soh_upi_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (field_id > 5) ||(parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  field_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	switch(field_id)
	{
		case 0:
			regp_field_write(chip_id,rst_soh_upi_125ext_n_reg[RST_SOH_UPI_125EXT_N],parameter);
			break;
		case 1:
			regp_field_write(chip_id,rst_soh_upi_125ins_n_reg[RST_SOH_UPI_125INS_N],parameter);
			break;
		case 2:
			regp_field_write(chip_id,rst_soam_upi_125ext_n_reg[RST_SOAM_UPI_125EXT_N],parameter);
			break;
		case 3:
			regp_field_write(chip_id,rst_soam_upi_125ins_n_reg[RST_SOAM_UPI_125INS_N],parameter);
			break;
		case 4:
			regp_field_write(chip_id,rst_soh_upi_fuci_n_reg[RST_SOH_UPI_FUCI_N],parameter);
			break;
		case 5:
			regp_field_write(chip_id,rst_soh_upi_fuci_n_reg[RST_SUBSYS_MAC_FUCI_N],parameter);
			break;
		default:
			break;
	}
	
	return RET_SUCCESS;

}

/******************************************************************************
*
* FUNCTION
*
*     top_clk_measure_cfg_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: field select 0:MEASURE_TIME 1:MEASURE_CLR
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-2-27    1.0           initial
*
******************************************************************************/
RET_STATUS top_clk_measure_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(clk_measure_cfg_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,clk_measure_cfg_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_clk_measure_cfg_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: field select 0:MEASURE_TIME 1:MEASURE_CLR
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-2-27    1.0           initial
*
******************************************************************************/
RET_STATUS top_clk_measure_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(clk_measure_cfg_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,clk_measure_cfg_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_measure_cnt_max_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  channel_id: channel select 0~12
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-2-27    1.0           initial
*
******************************************************************************/
RET_STATUS top_measure_cnt_max_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > 12))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n channel_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  channel_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,measure_cnt_max_reg[MEASURE_CNT_MAX].base_addr,measure_cnt_max_reg[MEASURE_CNT_MAX].offset_addr+channel_id,
		measure_cnt_max_reg[MEASURE_CNT_MAX].end_bit,measure_cnt_max_reg[MEASURE_CNT_MAX].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_measure_cnt_min_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  channel_id: channel select 0~12
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-2-27    1.0           initial
*
******************************************************************************/
RET_STATUS top_measure_cnt_min_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > 12))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n channel_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  channel_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,measure_cnt_min_reg[MEASURE_CNT_MIN].base_addr,measure_cnt_min_reg[MEASURE_CNT_MIN].offset_addr+channel_id,
		measure_cnt_min_reg[MEASURE_CNT_MIN].end_bit,measure_cnt_min_reg[MEASURE_CNT_MIN].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_sys_logic4_n_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: 0~16  0:RST_SYS_PORTSCH_UP_N  1:RST_INF_CH_ADP_RX_N
*					 2:RST_INF_CH_ADP_TX_N   3:RST_FLEXE_ENV_N  4:RST_REQ_GEN_N
*					 5:RST_MR_PCS_RX_0_N  6:RST_MR_PCS_RX_1_N  7:RST_MR_PCS_RX_2_N
*					 8:RST_MR_PCS_RX_3_N  9:RST_MR_PCS_TX_0_N  10:RST_MR_PCS_TX_1_N
*					 11:RST_MR_PCS_TX_2_N  12:RST_MR_PCS_TX_3_N  13:RST_FEC_DEC_0_N
*					 14:RST_FEC_DEC_1_N  15:RST_FEC_ENC_0_N  16:RST_FEC_ENC_1_N
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-04-18    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_sys_logic4_n_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_sys_logic4_n_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_sys_logic4_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_cdr_sel_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-4-24    1.0           initial
*
******************************************************************************/
RET_STATUS top_cdr_sel_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,cdr_sel_reg[CDR_SEL],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_cdr_sel_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-4-24    1.0           initial
*
******************************************************************************/
RET_STATUS top_cdr_sel_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,cdr_sel_reg[CDR_SEL],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_ieee1588_1_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:field select 0~25  0~15: RST_FLEXE_PTP_RX_0_N~RST_FLEXE_PTP_RX_15_N
*								  16~23: RST_FLEXE_PTP_TX_0_N~RST_FLEXE_PTP_TX_7_N
*								  24:RST_XGMII_RX_N  25:RST_XGMII_TX_N
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-5-14    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_ieee1588_1_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_ieee1588_1_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_ieee1588_1_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_rst_ieee1588_2_reset
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:field select 0~27  0~15: RST_UPI_FLEXE_PTP_RX_0_N~RST_UPI_FLEXE_PTP_RX_15_N
*								  16~23: RST_UPI_FLEXE_PTP_TX_0_N~RST_UPI_FLEXE_PTP_TX_7_N
*								  24:RST_UPI_GLB_1588_N  25:RST_UPI_XGMII_RX_N
*								  26:RST_UPI_XGMII_TX_N  27:RST_UPI_CCU_GE_N
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-5-14    1.0           initial
*
******************************************************************************/
RET_STATUS top_rst_ieee1588_2_reset(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(rst_ieee1588_2_reg))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,rst_ieee1588_2_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_int1_int_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: 0~7:OH_RX0_INT~OH_RX7_INT  8~19:FLEXE_OAM_RX0_INT~FLEXE_OAM_RX11_INT
*				20~22:FLEXE_OAM_TX0_INT~FLEXE_OAM_TX2_INT  23:SAR_RX_INT  24:SAR_TX_INT
*				25~31:SAR_OAM_RX0_INT~SAR_OAM_RX6_INT
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-6-5    1.0           initial
*
******************************************************************************/
RET_STATUS top_int1_int_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(top_int1_INT_REG))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,top_int1_INT_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_int1_int_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: 0~7:OH_RX0_INT~OH_RX7_INT  8~19:FLEXE_OAM_RX0_INT~FLEXE_OAM_RX11_INT
*				20~22:FLEXE_OAM_TX0_INT~FLEXE_OAM_TX2_INT  23:SAR_RX_INT  24:SAR_TX_INT
*				25~31:SAR_OAM_RX0_INT~SAR_OAM_RX6_INT
*     parameter: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-6-5    1.0           initial
*
******************************************************************************/
RET_STATUS top_int1_int_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d\r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(top_int1_INT_REG))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,top_int1_INT_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_int1_int_mask_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: 0~7:OH_RX0_INT~OH_RX7_INT  8~19:FLEXE_OAM_RX0_INT~FLEXE_OAM_RX11_INT
*				20~22:FLEXE_OAM_TX0_INT~FLEXE_OAM_TX2_INT  23:SAR_RX_INT  24:SAR_TX_INT
*				25~31:SAR_OAM_RX0_INT~SAR_OAM_RX6_INT
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-6-5    1.0           initial
*
******************************************************************************/
RET_STATUS top_int1_int_mask_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(top_int1_INT_MASK_REG))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,top_int1_INT_MASK_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_int1_int_mask_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: 0~7:OH_RX0_INT~OH_RX7_INT  8~19:FLEXE_OAM_RX0_INT~FLEXE_OAM_RX11_INT
*				20~22:FLEXE_OAM_TX0_INT~FLEXE_OAM_TX2_INT  23:SAR_RX_INT  24:SAR_TX_INT
*				25~31:SAR_OAM_RX0_INT~SAR_OAM_RX6_INT
*     parameter: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-6-5    1.0           initial
*
******************************************************************************/
RET_STATUS top_int1_int_mask_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(top_int1_INT_MASK_REG))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,top_int1_INT_MASK_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_int2_int_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: 1~5:SAR_OAM_RX7_INT~SAR_OAM_RX11_INT  6~8:SAR_OAM_TX0_INT~SAR_OAM_TX2_INT
*				9:IEEE_1588_INT  10:SAR_OAM_RX12_INT  11:FLEXE_OAM_RX12_INT
*				12:SAR_OAM_RX13_INT  13:FLEXE_OAM_RX13_INT
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-6-5    1.0           initial
*
******************************************************************************/
RET_STATUS top_int2_int_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(top_int2_INT_REG))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,top_int2_INT_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_int2_int_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: 1~5:SAR_OAM_RX7_INT~SAR_OAM_RX11_INT  6~8:SAR_OAM_TX0_INT~SAR_OAM_TX2_INT
*				9:IEEE_1588_INT  10:SAR_OAM_RX12_INT  11:FLEXE_OAM_RX12_INT
*				12:SAR_OAM_RX13_INT  13:FLEXE_OAM_RX13_INT
*     parameter: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-6-5    1.0           initial
*
******************************************************************************/
RET_STATUS top_int2_int_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(top_int2_INT_REG))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,top_int2_INT_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_int2_int_mask_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: 1~5:SAR_OAM_RX7_INT~SAR_OAM_RX11_INT  6~8:SAR_OAM_TX0_INT~SAR_OAM_TX2_INT
*				9:IEEE_1588_INT  10:SAR_OAM_RX12_INT  11:FLEXE_OAM_RX12_INT
*				12:SAR_OAM_RX13_INT  13:FLEXE_OAM_RX13_INT
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-6-5    1.0           initial
*
******************************************************************************/
RET_STATUS top_int2_int_mask_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(top_int2_INT_MASK_REG))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,top_int2_INT_MASK_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     top_int2_int_mask_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: 1~5:SAR_OAM_RX7_INT~SAR_OAM_RX11_INT  6~8:SAR_OAM_TX0_INT~SAR_OAM_TX2_INT
*				9:IEEE_1588_INT  10:SAR_OAM_RX12_INT  11:FLEXE_OAM_RX12_INT
*				12:SAR_OAM_RX13_INT  13:FLEXE_OAM_RX13_INT
*     parameter: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-6-5    1.0           initial
*
******************************************************************************/
RET_STATUS top_int2_int_mask_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (top_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (top_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(top_int2_INT_MASK_REG))
	{
		if (top_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,top_int2_INT_MASK_REG[field_id],parameter);
	
	return RET_SUCCESS;
}
