

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

#ifndef REG_OPERATE_H
#define REG_OPERATE_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "datatype.h"
#include "hal.h"

/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
/******************************************************************************
*
* MOUDLE_NAME
*
*	 BASE_ADDRESS
*
* MOUDEL_ADDRESS
*
*	 BASE_ADDRESS
*
* MOUDLE_OFFSET
*
*    BASE_ADDRESS
*
* MOUDLE_RegisterName
*
*    BASE_ADDRESS
*
******************************************************************************/
#define TOP_BASE_ADDR 0x0
#define TABLE_BASE_ADDR 0x20000 /*change for asic*/
#define TABLE_BASE_ADDR_FOR_FPGA 0x20000 /*outside of the core*/
#define MCMACRX_BASE_ADDR 0x8000  /*change for asic*/
#define MCMACTX_BASE_ADDR 0x8100  /*change for asic*/
#define B66SAR_RX_BASE_ADDR 0x8400 /*change for asic*/
#define B66SAR_TX_BASE_ADDR 0x8500 /*change for asic*/
#define B66SAR_RX_311M_BASE_ADDR 0x8600 /*change for asic*/
#define B66SAR_TX_311M_BASE_ADDR 0x8700 /*change for asic*/
#define CPB_EGRESS_BASE_ADDR 0x8800 /*change for asic*/
#define FLEXE_OH_RX_BASE_ADDR 0x14000 /*change for asic*/
#define FLEXE_OH_TX_BASE_ADDR 0x14200  /*change for asic*/
#define FLEXE_OFFSET 0x40
#define FLEXE_MULTI_DESKEW_BASE_ADDR 0x14400 /*change for asic*/
#define FLEXE_MUX_BASE_ADDR 0xc200  /*change for asic*/
#define FLEXE_DEMUX_BASE_ADDR 0xc400   /*change for asic*/
#define RATEADP_BASE_ADDR 0xc600    /*change for asic*/
#define ADJ_BASE_ADDR 0xc700		 /*change for asic*/
#define CHANNELIZE_BASE_ADDR 0xc800  /*change for asic*/
#define FLEXE_MACRX_BASE_ADDR 0xca00  /*change for asic*/
#define FLEXE_OAM_RX_BASE_ADDR 0xcc00  /*change for asic*/
#define FLEXE_OAM_TX_BASE_ADDR 0xcd00  /*change for asic*/
#define OAM_RX_BASE_ADDR 0x8c00 /*change for asic*/
#define OAM_TX_BASE_ADDR 0x8d00 /*change for asic*/
#define MAC_ADAPT_BASE_ADDR 0x9000  /*change for asic*/
#define MAC_ADJ_BASE_ADDR 0x9100	 /*change for asic*/
#define OHOAM_EF_BASE_ADDR 0xe000  /*change for asic*/
#define OHOAM_EF_ASIC_BASE_ADDR 0xe080  /*change for asic*/
#define B66SWITCH_BASE_ADDR 0xa200 /*change for asic*/
#define CPU_BASE_ADDR 0x16200  /*change for asic*/
#define CCU_CFG_BASE_ADDR 0x16400 /*change for asic*/
#define CCU_RX_BASE_ADDR 0x16480  /*change for asic*/
#define CCU_TX_BASE_ADDR 0x16500  /*change for asic*/
#define GLOBAL_IEEE1588_BASE_ADDR 0x16000  /*change for asic*/
#define PTP_TX_OH_BASE_ADDR 0x16010 /*change for asic*/
#define PTP_TX_OH_OFFSET 0x10
#define PTP_RX_OH_BASE_ADDR 0x16090  /*change for asic*/
#define PTP_RX_OH_OFFSET 0x10
#define PPS_TOD_GLOBAL_TS_BASE_ADDR 0x16600  /*change for asic*/
#define INF_CH_ADP_RX_BASE_ADDR 0x18000  /*change for asic*/
#define INF_CH_ADP_TX_BASE_ADDR 0x18080  /*change for asic*/
#define FLEXE_ENV_BASE_ADDR 0x18100   /*change for asic*/
#define REQ_GEN_BASE_ADDR 0x18200  /*change for asic*/

/******************************************************************************
*
* MOUDLE_NAME
*
*	 b66sar_rx
*
* MOUDEL_ADDRESS
*
*	 b66sar_rx
*
* MOUDLE_OFFSET
*
*    b66sar_rx
*
* MOUDLE_RegisterName
*
*    b66sar_rx
*
*
******************************************************************************/
/********b66sar_rx***************/
#define RX_LOOPBACK 0
#define B66SIZE_29B 1
#define RX_PKT_CNT_PROB 2
#define RX_PKT_CNT_256_PROB 3
#define BDCOME_FC_EN 4
#define BSIZE_ERR 0
#define ADJ_IN_ALM 1
#define I1_OVERFLOW 2
#define I1_UNDERFLOW 3
#define I2_OVERFLOW 4
#define I2_UNDERFLOW 5
#define BT_SOP_ERR 6
#define BT_CHAN_ERR 7
#define MON_EN 0
#define CPB2SAR_CH_EN 1
#define BRCM_FC_CH_EN 2
#define RX_CHAN_MAP 3
#define RX_FIFO_HIGH_CFG 4
#define RX_FIFO_LOW_CFG 5
#define RX_FIFO_MID_CFG 6
#define CPB2SAR_FIFO_ADJ_PERIOD 0
#define SAR_FIFO_ADJ_PERIOD 1
#define CPB2SAR_CFG_PERIOD 0
#define CPB2SAR_NOM_CNT 1
#define CPB2SAR_FIFO_ADJ_EN 2
#define SAR_FIFO_ADJ_EN 3
#define RX_PKT_CNT 0
#define RX_PKT_CNT_256 0
#define CPB2SAR_Q 0
#define CPB2SAR_ADJ_01EN 1
#define CPB2SAR_CALENDAR_EN 2
#define CPB2SAR_ADJ_IN_CHECK_VALUE 3
#define CPB2SAR_ADJ_IN_CORRECT_EN 4
#define RX_PATH_DELAY 0
#define CSI 1
#define CPB2SAR_CURRENT_TABLE 2
#define CPB2SAR_BUSY 3
#define RX_JOINT_INIT_DONE 4
#define CPB2SAR_FIFO_ADJ_INIT_DONE 5
#define SAR_FIFO_ADJ_INIT_DONE 6
#define CPB2SAR_DECIMATE_INIT_DONE 7
#define CPB2SAR_TABLE_SWITCH 0 
#define RX_JOINT_INFO_INIT 1
#define CPB2SAR_FIFO_ADJ_INIT 2
#define SAR_FIFO_ADJ_INIT 3
#define CPB2SAR_DECIMATE_INIT 4
#define CPB2SAR_LOCAL_EN 0
#define CPB2SAR_LOCAL_M 1
#define DELAY_CYCLES 2
#define RX_PAR_ERR 0
#define RX_DROP_ONE_ERR 1
#define RX_DROP_TWO_ERR 2
#define RX_FIFO_HIGH_IND 3
#define RX_FIFO_LOW_IND 4
#define RX_FIFO_FULL 5
#define RX_FIFO_EMPTY 6
#define BT_SOP_ERR_INT 0
#define BT_CHAN_ERR_INT 1
#define BT_SOP_ERR_INT_MASK 0
#define BT_CHAN_ERR_INT_MASK 1
#define RX_FIFO_ERR_0_INT 0
#define RX_FIFO_ERR_1_INT 1
#define RX_FIFO_ERR_2_INT 2
#define RX_FIFO_ERR_3_INT 3
#define RX_FIFO_ERR_4_INT 4
#define RX_FIFO_ERR_5_INT 5
#define RX_FIFO_ERR_6_INT 6
#define RX_FIFO_ERR_7_INT 7
#define RX_FIFO_ERR_8_INT 8
#define RX_FIFO_ERR_9_INT 9
#define RX_FIFO_ERR_10_INT 10
#define RX_FIFO_ERR_11_INT 11
#define RX_FIFO_ERR_12_INT 12
#define RX_FIFO_ERR_13_INT 13
#define RX_FIFO_ERR_14_INT 14
#define RX_FIFO_ERR_15_INT 15
#define RX_FIFO_ERR_0_INT_MASK 0
#define RX_FIFO_ERR_1_INT_MASK 1
#define RX_FIFO_ERR_2_INT_MASK 2
#define RX_FIFO_ERR_3_INT_MASK 3
#define RX_FIFO_ERR_4_INT_MASK 4
#define RX_FIFO_ERR_5_INT_MASK 5
#define RX_FIFO_ERR_6_INT_MASK 6
#define RX_FIFO_ERR_7_INT_MASK 7
#define RX_FIFO_ERR_8_INT_MASK 8
#define RX_FIFO_ERR_9_INT_MASK 9
#define RX_FIFO_ERR_10_INT_MASK 10
#define RX_FIFO_ERR_11_INT_MASK 11
#define RX_FIFO_ERR_12_INT_MASK 12
#define RX_FIFO_ERR_13_INT_MASK 13
#define RX_FIFO_ERR_14_INT_MASK 14
#define RX_FIFO_ERR_15_INT_MASK 15

/******************************************************************************
*
* MOUDLE_NAME
*
*	 b66sar_tx
*
* MOUDEL_ADDRESS
*
*	 b66sar_tx
*
* MOUDLE_OFFSET
*
*    b66sar_tx
*
* MOUDLE_RegisterName
*
*	 b66sar_tx
*
*
******************************************************************************/
/********b66sar_tx***************/
#define TX_LOOPBACK 0
#define TX_B66SIZE_29B 1
#define TX_CNT_PROB 2
#define TX_BT256_CNT_PROB 3
#define BDCOME_LOOPBACK 4
#define RA2SAR_CFG_PERIOD 0
#define RA2SAR_NOM_CNT 1
#define SEG_EN 0
#define RA2SAR_CH_EN 1
#define TX_CHAN_MAP 2
#define RA2SAR_Q 0
#define RA2SAR_ADJ_01EN 1
#define RA2SAR_CALENDAR_EN 2
#define RA2SAR_ADJ_IN_CHECK_VALUE 3
#define RA2SAR_ADJ_IN_CORRECT_EN 4
#define TX_PKT_CNT 0
#define TX_PKT_CNT_256 0
#define RA2SAR_TABLE_SWITCH 0
#define SEG_FSM_STATE 0
#define RA2SAR_CURRENT_TABLE 1
#define RA2SAR_BUSY 2
#define TX_ADJ_IN_ALM 0
#define TX_I1_OVERFLOW 1
#define TX_I1_UNDERFLOW 2
#define TX_I2_OVERFLOW 3
#define TX_I2_UNDERFLOW 4
#define TX_BTFIFO_FULL 5
#define RA2SAR_LOCAL_EN 0
#define RA2SAR_LOCAL_M 1
#define TX_BTFIFO_FULL_INT 0
#define TX_BTFIFO_FULL_INT_MASK 0

/******************************************************************************
*
* MOUDLE_NAME
*
*	 b66sar_rx_311m
*
* MOUDEL_ADDRESS
*
*	 b66sar_rx_311m
*
* MOUDLE_OFFSET
*
*    b66sar_rx_311m
*
* MOUDLE_RegisterName
*
*    b66sar_rx_311m
*
*
******************************************************************************/
/**********b66sar_rx_311m***********/
#define CPB2SAR_M_1ST 0
#define CPB2SAR_BASE_M_1ST 0
#define CPB2SAR_MULTIPLIER 0
#define CPB2SAR_BYPASS_M_N_2ND 1
#define CPB2SAR_M_2ND 0
#define CPB2SAR_BASE_M_2ND 0

/******************************************************************************
*
* MOUDLE_NAME
*
*	 b66sar_tx_311m
*
* MOUDEL_ADDRESS
*
*	 b66sar_tx_311m
*
* MOUDLE_OFFSET
*
*    b66sar_tx_311m
*
* MOUDLE_RegisterName
*
*	 b66sar_tx_311m
*
*
******************************************************************************/
/**********b66sar_tx_311m***********/
#define RA2SAR_M_1ST 0
#define RA2SAR_BASE_M_1ST 0
#define RA2SAR_MULTIPLIER 0
#define RA2SAR_BYPASS_M_N_2ND 1
#define RA2SAR_M_2ND 0
#define RA2SAR_BASE_M_2ND 0

/******************************************************************************
*
* MOUDLE_NAME
*
*	 cpb_egress
*
* MOUDEL_ADDRESS
*
*	 cpb_egress
*
* MOUDLE_OFFSET
*
*    cpb_egress
*
* MOUDLE_RegisterName
*
*	 cpb_egress
*
*
******************************************************************************/
/***********cpb_egress************/
#define SEL_TB_EN 0
#define CLR_EG 0
#define SQ_ERR_EG_0 0
#define SQ_ERR_EG_1 1
#define SQ_ERR_EG_2 2
#define MON_EN_EG_0 0
#define MON_EN_EG_1 1
#define MON_EN_EG_2 2
#define CNT_MAX_EG 0
#define CNT_DEPTH_EG 0
#define CNT_MIN_EG 1
#define STA_CLR_CH_EG 0
#define DLY_FIFO_STATE 0
#define DATA_RAM_ADDR_ALM 1
#define LINK_FULL_EG_0 0
#define LINK_FULL_EG_1 1
#define LINK_FULL_EG_2 2
#define LINK_EMPTY_EG_0 0
#define LINK_EMPTY_EG_1 1
#define LINK_EMPTY_EG_2 2
#define CH0_FULL_INT 0
#define CH1_FULL_INT 1
#define CH2_FULL_INT 2
#define CH3_FULL_INT 3
#define CH4_FULL_INT 4
#define CH5_FULL_INT 5
#define CH6_FULL_INT 6
#define CH7_FULL_INT 7
#define CH8_FULL_INT 8
#define CH9_FULL_INT 9
#define CH10_FULL_INT 10
#define CH11_FULL_INT 11
#define CH12_FULL_INT 12
#define CH13_FULL_INT 13
#define CH14_FULL_INT 14
#define CH15_FULL_INT 15
#define CH0_FULL_INT_MASK 0
#define CH1_FULL_INT_MASK 1
#define CH2_FULL_INT_MASK 2
#define CH3_FULL_INT_MASK 3
#define CH4_FULL_INT_MASK 4
#define CH5_FULL_INT_MASK 5
#define CH6_FULL_INT_MASK 6
#define CH7_FULL_INT_MASK 7
#define CH8_FULL_INT_MASK 8
#define CH9_FULL_INT_MASK 9
#define CH10_FULL_INT_MASK 10
#define CH11_FULL_INT_MASK 11
#define CH12_FULL_INT_MASK 12
#define CH13_FULL_INT_MASK 13
#define CH14_FULL_INT_MASK 14
#define CH15_FULL_INT_MASK 15
#define CH16_FULL_INT 0
#define CH17_FULL_INT 1
#define CH18_FULL_INT 2
#define CH19_FULL_INT 3
#define CH20_FULL_INT 4
#define CH21_FULL_INT 5
#define CH22_FULL_INT 6
#define CH23_FULL_INT 7
#define CH24_FULL_INT 8
#define CH25_FULL_INT 9
#define CH26_FULL_INT 10
#define CH27_FULL_INT 11
#define CH28_FULL_INT 12
#define CH29_FULL_INT 13
#define CH30_FULL_INT 14
#define CH31_FULL_INT 15
#define CH16_FULL_INT_MASK 0
#define CH17_FULL_INT_MASK 1
#define CH18_FULL_INT_MASK 2
#define CH19_FULL_INT_MASK 3
#define CH20_FULL_INT_MASK 4
#define CH21_FULL_INT_MASK 5
#define CH22_FULL_INT_MASK 6
#define CH23_FULL_INT_MASK 7
#define CH24_FULL_INT_MASK 8
#define CH25_FULL_INT_MASK 9
#define CH26_FULL_INT_MASK 10
#define CH27_FULL_INT_MASK 11
#define CH28_FULL_INT_MASK 12
#define CH29_FULL_INT_MASK 13
#define CH30_FULL_INT_MASK 14
#define CH31_FULL_INT_MASK 15
#define CH32_FULL_INT 0
#define CH33_FULL_INT 1
#define CH34_FULL_INT 2
#define CH35_FULL_INT 3
#define CH36_FULL_INT 4
#define CH37_FULL_INT 5
#define CH38_FULL_INT 6
#define CH39_FULL_INT 7
#define CH40_FULL_INT 8
#define CH41_FULL_INT 9
#define CH42_FULL_INT 10
#define CH43_FULL_INT 11
#define CH44_FULL_INT 12
#define CH45_FULL_INT 13
#define CH46_FULL_INT 14
#define CH47_FULL_INT 15
#define CH32_FULL_INT_MASK 0
#define CH33_FULL_INT_MASK 1
#define CH34_FULL_INT_MASK 2
#define CH35_FULL_INT_MASK 3
#define CH36_FULL_INT_MASK 4
#define CH37_FULL_INT_MASK 5
#define CH38_FULL_INT_MASK 6
#define CH39_FULL_INT_MASK 7
#define CH40_FULL_INT_MASK 8
#define CH41_FULL_INT_MASK 9
#define CH42_FULL_INT_MASK 10
#define CH43_FULL_INT_MASK 11
#define CH44_FULL_INT_MASK 12
#define CH45_FULL_INT_MASK 13
#define CH46_FULL_INT_MASK 14
#define CH47_FULL_INT_MASK 15
#define CH48_FULL_INT 0
#define CH49_FULL_INT 1
#define CH50_FULL_INT 2
#define CH51_FULL_INT 3
#define CH52_FULL_INT 4
#define CH53_FULL_INT 5
#define CH54_FULL_INT 6
#define CH55_FULL_INT 7
#define CH56_FULL_INT 8
#define CH57_FULL_INT 9
#define CH58_FULL_INT 10
#define CH59_FULL_INT 11
#define CH60_FULL_INT 12
#define CH61_FULL_INT 13
#define CH62_FULL_INT 14
#define CH63_FULL_INT 15
#define CH48_FULL_INT_MASK 0
#define CH49_FULL_INT_MASK 1
#define CH50_FULL_INT_MASK 2
#define CH51_FULL_INT_MASK 3
#define CH52_FULL_INT_MASK 4
#define CH53_FULL_INT_MASK 5
#define CH54_FULL_INT_MASK 6
#define CH55_FULL_INT_MASK 7
#define CH56_FULL_INT_MASK 8
#define CH57_FULL_INT_MASK 9
#define CH58_FULL_INT_MASK 10
#define CH59_FULL_INT_MASK 11
#define CH60_FULL_INT_MASK 12
#define CH61_FULL_INT_MASK 13
#define CH62_FULL_INT_MASK 14
#define CH63_FULL_INT_MASK 15
#define CH64_FULL_INT 0
#define CH65_FULL_INT 1
#define CH66_FULL_INT 2
#define CH67_FULL_INT 3
#define CH68_FULL_INT 4
#define CH69_FULL_INT 5
#define CH70_FULL_INT 6
#define CH71_FULL_INT 7
#define CH72_FULL_INT 8
#define CH73_FULL_INT 9
#define CH74_FULL_INT 10
#define CH75_FULL_INT 11
#define CH76_FULL_INT 12
#define CH77_FULL_INT 13
#define CH78_FULL_INT 14
#define CH79_FULL_INT 15
#define CH64_FULL_INT_MASK 0
#define CH65_FULL_INT_MASK 1
#define CH66_FULL_INT_MASK 2
#define CH67_FULL_INT_MASK 3
#define CH68_FULL_INT_MASK 4
#define CH69_FULL_INT_MASK 5
#define CH70_FULL_INT_MASK 6
#define CH71_FULL_INT_MASK 7
#define CH72_FULL_INT_MASK 8
#define CH73_FULL_INT_MASK 9
#define CH74_FULL_INT_MASK 10
#define CH75_FULL_INT_MASK 11
#define CH76_FULL_INT_MASK 12
#define CH77_FULL_INT_MASK 13
#define CH78_FULL_INT_MASK 14
#define CH79_FULL_INT_MASK 15
#define DEBUG_CHAN 0
#define STA_CLR_EG 0
#define DEBUG_CLR 1
#define MIN_PATHDELAY 0
#define MAX_PATHDELAY 1
#define REAL_PATHDELAY 0
#define STA_CLR_DONE 1
#define CPB_ILK_BP_0 0
#define CPB_ILK_BP_1 1
#define CPB_ILK_BP_2 2

/******************************************************************************
*
* MOUDLE_NAME
*
*	 flexe_oh_rx
*
* MOUDEL_ADDRESS
*
*	 flexe_oh_rx
*
* MOUDLE_OFFSET
*
*    flexe_oh_rx
*
* MOUDLE_RegisterName
*
*	 flexe_oh_rx
*
*
******************************************************************************/
/**************flexe_oh_rx*****************/
#define TEST_EN 0
#define OHRX_DIN_SRC_CFG 1
#define MUX2OHTX_CP_CFG 2
#define FLEXE_RX_MODE 0
#define MF16T32_RX_CFG 1
#define OMF_CHK_CTRL 2
#define OMF_CRC_CTRL 3
#define ALM_EN 0
#define CRC_RX_MODE 1
#define EXCCC 0
#define EXSC 1
#define EXGID 0
#define EXPID 1
#define OOF_SET_CFG 0
#define OOF_CLR_CFG 1
#define LOF_SET_CFG 2
#define LOF_CLR_CFG 3
#define FP_PATTERN_CFG 4
#define OOM_SET_CFG 0
#define OOM_CLR_CFG 1
#define LOM_SET_CFG 2
#define LOM_CLR_CFG 3
#define OOP_SET_CFG 0
#define OOP_CLR_CFG 1
#define LOP_SET_CFG 2
#define LOP_CLR_CFG 3
#define PAD_ALM_EN 0
#define PCS_LF_EN 1
#define CRC_ALM_EN 2
#define ACCCC 0
#define ACCR 1
#define ACCA 2
#define ACSC 3
#define ACGID 0
#define ACPID 1
#define ACMC_SETION_0 0
#define ACMC_SETION_1 1
#define ACMC_SETION_2 2
#define ACMC_SETION_3 3
#define AC_SHIM2SHIM_0 0
#define AC_SHIM2SHIM_1 1
#define AC_SHIM2SHIM_2 2
#define AC_SHIM2SHIM_3 3
#define AC_SHIM2SHIM_4 4
#define AC_SHIM2SHIM_5 5
#define AC_RESERVED1_0 0
#define AC_RESERVED1_1 1
#define AC_RESERVED2 2
#define OOF_ALM 0
#define LOF_ALM 1
#define OOM_ALM 2
#define LOM_ALM 3
#define CRC_ALM 4
#define GIDM_ALM 5
#define PMM_ALM 6
#define RPF_ALM 7
#define SCM_ALM 8
#define OOP_ALM 9
#define LOP_ALM 10
#define PIDM_ALM 11
#define OH1_BLOCK_ALM 12
#define C_BIT_ALM 13
#define GID_UNEQ_ALM 14
#define CCM 0
#define LOF_ALM_INT     0
#define LOM_ALM_INT     1
#define GIDM_ALM_INT    2
#define PIDM_ALM_INT    3
#define PMM_ALM_INT     4
#define RPF_ALM_INT     5
#define DESKEW_ALM_INT  6
#define CCM_ALM0_INT    7
#define CCM_ALM1_INT    8
#define CCM_ALM2_INT    9
#define CCM_ALM3_INT    10
#define CCM_ALM4_INT    11
#define CCM_ALM5_INT    12
#define CCM_ALM6_INT    13
#define CCM_ALM7_INT    14
#define CCM_ALM8_INT    15
#define CCM_ALM9_INT    16
#define CCM_ALM10_INT   17
#define CCM_ALM11_INT   18
#define CCM_ALM12_INT   19
#define CCM_ALM13_INT   20
#define CCM_ALM14_INT   21
#define CCM_ALM15_INT   22
#define CCM_ALM16_INT   23
#define CCM_ALM17_INT   24
#define CCM_ALM18_INT   25
#define CCM_ALM19_INT   26
#define CR_STA_INT  27
#define CA_STA_INT  28
#define LOF_ALM_INT_MASK     0
#define LOM_ALM_INT_MASK     1
#define GIDM_ALM_INT_MASK    2
#define PIDM_ALM_INT_MASK    3
#define PMM_ALM_INT_MASK     4
#define RPF_ALM_INT_MASK     5
#define DESKEW_ALM_INT_MASK  6
#define CCM_ALM0_INT_MASK    7
#define CCM_ALM1_INT_MASK    8
#define CCM_ALM2_INT_MASK    9
#define CCM_ALM3_INT_MASK    10
#define CCM_ALM4_INT_MASK    11
#define CCM_ALM5_INT_MASK    12
#define CCM_ALM6_INT_MASK    13
#define CCM_ALM7_INT_MASK    14
#define CCM_ALM8_INT_MASK    15
#define CCM_ALM9_INT_MASK    16
#define CCM_ALM10_INT_MASK   17
#define CCM_ALM11_INT_MASK   18
#define CCM_ALM12_INT_MASK   19
#define CCM_ALM13_INT_MASK   20
#define CCM_ALM14_INT_MASK   21
#define CCM_ALM15_INT_MASK   22
#define CCM_ALM16_INT_MASK   23
#define CCM_ALM17_INT_MASK   24
#define CCM_ALM18_INT_MASK   25
#define CCM_ALM19_INT_MASK   26
#define CR_STA_INT_MASK  27
#define CA_STA_INT_MASK  28

/******************************************************************************
*
* MOUDLE_NAME
*
*	 flexe_oh_tx
*
* MOUDEL_ADDRESS
*
*	 flexe_oh_tx
*
* MOUDLE_OFFSET
*
*    flexe_oh_tx
*
* MOUDLE_RegisterName
*
* 	flexe_oh_tx
*
*
******************************************************************************/
/**************flexe_oh_tx*****************/
#define FLEXE_TX_MODE 0
#define CRC_TX_MODE 1
#define UNEQUIPPED_CFG 2
#define B66_ERR_RPL 3
#define MF16T32_TX_CFG 4
#define PHY_INST_TX_RI_SW_CFG 5
#define SMC_REQ_CFG 6
#define FAS_SRC_CFG 0
#define CC_SRC_CFG 1
#define OMF_SRC_CFG 2
#define RPF_SRC_CFG 3
#define GID_SRC_CFG 4
#define PYHMAP_SRC_CFG 5
#define PID_SRC_CFG 6
#define CCA_SRC_CFG 7
#define CCB_SRC_CFG 8
#define CR_SRC_CFG 9
#define CA_SRC_CFG 10
#define CRC16_SRC_CFG 11
#define SECTION_SRC_CFG 12
#define SHIM2SHIM_SRC_CFG 13
#define SMC_SRC_CFG 14
#define SC_SRC_CFG 15
#define RESERVED1_SRC_CFG 0
#define RESERVED2_SRC_CFG 1
#define CCC_INS 0
#define CR_INS 1
#define CA_INS 2
#define RPF_INS 3
#define PID_INS 4
#define GID_INS 5
#define MC_SECTION_INS_0 0
#define MC_SECTION_INS_1 1
#define MC_SECTION_INS_2 2
#define MC_SECTION_INS_3 3
#define SMC_INS_0 0
#define SMC_INS_1 1
#define MC_S2S_INS_0 2
#define MC_S2S_INS_1 3
#define MC_S2S_INS_2 4
#define MC_S2S_INS_3 5
#define SC_INS 0
#define RESERVED1_INS_0 1
#define RESERVED1_INS_1 2
#define RESERVED2_INS 3
#define OH_SH 0
#define B66CODE_ERR 0

/******************************************************************************
*
* MOUDLE_NAME
*
*	 flexe_multi_deskew
*
* MOUDEL_ADDRESS
*
*	 flexe_multi_deskew
*
* MOUDLE_OFFSET
*
*    flexe_multi_deskew
*
* MOUDLE_RegisterName
*
* 	flexe_multi_deskew
*
*
******************************************************************************/
/*************flexe_multi_deskew****************/
#define GRP_SORT_CFG0 0
#define GRP_SORT_CFG1 1
#define GRP_SORT_CFG2 2
#define GRP_SORT_CFG3 3
#define GRP_SORT_CFG4 0
#define GRP_SORT_CFG5 1
#define GRP_SORT_CFG6 2
#define GRP_SORT_CFG7 3
#define DSW_MARGIN_CFG0 0
#define DSW_MARGIN_CFG1 1
#define DSW_MARGIN_CFG2 0
#define DSW_MARGIN_CFG3 1
#define DSW_MARGIN_CFG4 0
#define DSW_MARGIN_CFG5 1
#define DSW_MARGIN_CFG6 0
#define DSW_MARGIN_CFG7 1
#define GRP_SSF_EN 0
#define PHY_SSF_EN 1
#define GRP_INI_SEL 0
#define GRP_INI_CFG 1
#define FIFO_ADDR_OFFSET 2
#define GRP_MODE 0
#define BYPASS_50G 1
#define BYPASS_100G 2
#define DESKEW_RATE_M_50G 0
#define DESKEW_RATE_BASE_M_50G 0
#define DESKEW_RATE_M_100G 0
#define DESKEW_RATE_BASE_M_100G 0
#define DESKEW_ALM_EN0 0
#define DESKEW_ALM_EN1 1
#define DESKEW_ALM_EN2 2
#define DSW_MEASURE_GRP0 0
#define DSW_MEASURE_GRP1 1
#define DSW_MEASURE_GRP2 0
#define DSW_MEASURE_GRP3 1
#define DSW_MEASURE_GRP4 0
#define DSW_MEASURE_GRP5 1
#define DSW_MEASURE_GRP6 0
#define DSW_MEASURE_GRP7 1
#define GRP_DSW_FIFO_EMPTY 0
#define GRP_DSW_FIFO_FULL 1
#define GRP_DSW_OVERMARGIN 2
#define GRP_DSW_ALM 3
#define GRP_SSF_ALM 0
#define PHY200G_INST_NUM_ALM 1
#define PHY400G_INST_NUM_ALM 2

/******************************************************************************
*
* MOUDLE_NAME
*
*	 flexe_mux
*
* MOUDEL_ADDRESS
*
*	 flexe_mux
*
* MOUDLE_OFFSET
*
*    flexe_mux
*
* MOUDLE_RegisterName
*
*	 flexe_mux
*
*
******************************************************************************/
/****************flexe_mux****************/
#define FLEXE_MODE 0
#define FLEXE_ENV_MODE 1
#define FLEXE_GROUP_CFG 0
#define FLEXE_SCH_CFG 0
#define FLEXE_MSI_CFG_0 0
#define FLEXE_MSI_CFG_1 1
#define FLEXE_MSI_CFG_2 2
#define FLEXE_MSI_CFG_3 3
#define TSNUM_CFG_0 0
#define TSNUM_CFG_1 1
#define TSNUM_CFG_2 2
#define TSNUM_CFG_3 3
#define CYCLE_CFG_0 0
#define CYCLE_CFG_1 1
#define CYCLE_CFG_2 2
#define CYCLE_CFG_3 3
#define BLOCK_CFG_0 0
#define BLOCK_CFG_1 1
#define BLOCK_CFG_2 2
#define BLOCK_CFG_3 3
#define BLOCK_CFG_4 4
#define BLOCK_CFG_5 5
#define BLOCK_CFG_6 6
#define BLOCK_CFG_7 7
#define BLOCK_CFG_8 8
#define BLOCK_CFG_9 9
#define CH_BELONG_FLEXE_0 0
#define CH_BELONG_FLEXE_1 1
#define CH_BELONG_FLEXE_2 2
#define CH_BELONG_FLEXE_3 3
#define CH_BELONG_FLEXE_4 4
#define CH_BELONG_FLEXE_5 5
#define CH_BELONG_FLEXE_6 6
#define CH_BELONG_FLEXE_7 7
#define CH_BELONG_FLEXE_8 8
#define CH_BELONG_FLEXE_9 9
#define CH_BELONG_FLEXE_10 10
#define CH_BELONG_FLEXE_11 11
#define CH_BELONG_FLEXE_12 12
#define CH_BELONG_FLEXE_13 13
#define CH_BELONG_FLEXE_14 14
#define CH_BELONG_FLEXE_15 15
#define CH_BELONG_FLEXE_16 16
#define CH_BELONG_FLEXE_17 17
#define CH_BELONG_FLEXE_18 18
#define CH_BELONG_FLEXE_19 19
#define FLEXE_ENABLE 0
#define PCS_ENABLE 1
#define PCS_NUM 2
#define PCS_SCH_CFG 0
#define PCS_CHID_CFG_0 0
#define PCS_CHID_CFG_1 1
#define PCS_CHID_CFG_2 2
#define PCS_CHID_CFG_3 3
#define REQCNT_SLVL_CFG 0
#define PCSREQCNT_SLVL_CFG 1
#define C_CFG 0
#define FLEXE_MSI_CFG_B0 0
#define FLEXE_MSI_CFG_B1 1
#define FLEXE_MSI_CFG_B2 2
#define FLEXE_MSI_CFG_B3 3
#define TSNUM_CFG_B0 0
#define TSNUM_CFG_B1 1
#define TSNUM_CFG_B2 2
#define TSNUM_CFG_B3 3
#define CYCLE_CFG_B0 0
#define CYCLE_CFG_B1 1
#define CYCLE_CFG_B2 2
#define CYCLE_CFG_B3 3
#define BLOCK_CFG_B_0 0
#define BLOCK_CFG_B_1 1
#define BLOCK_CFG_B_2 2
#define BLOCK_CFG_B_3 3
#define BLOCK_CFG_B_4 4
#define BLOCK_CFG_B_5 5
#define BLOCK_CFG_B_6 6
#define BLOCK_CFG_B_7 7
#define BLOCK_CFG_B_8 8
#define BLOCK_CFG_B_9 9
#define TS_ENABLE_B_0 0
#define TS_ENABLE_B_1 1
#define TS_ENABLE_B_2 2
#define CPHY2_CFG_0 0
#define CPHY2_CFG_1 1
#define CPHY2_CFG_2 2
#define CPHY2_CFG_3 3
#define CPHY2_CFG_4 4
#define CPHY2_CFG_5 5
#define CPHY2_CFG_6 6
#define CPHY2_CFG_7 7
#define CNT8_SCH_CFG_0 0
#define CNT8_SCH_CFG_1 1
#define CNT8_SCH_CFG_2 2
#define CNT8_SCH_CFG_3 3
#define CNT8_SCH_CFG_4 4
#define CNT8_SCH_CFG_5 5
#define CNT8_SCH_CFG_6 6
#define CNT8_SCH_CFG_7 7
#define M_CFG_MUX 0
#define BASE_M_CFG_MUX 0
#define EN_SEL_CFG_0 0
#define EN_SEL_CFG_1 1
#define EN_SEL_CFG_2 2
#define EN_SEL_CFG_3 3
#define EN_SEL_CFG_4 4
#define EN_SEL_CFG_5 5
#define EN_SEL_CFG_6 6
#define EN_SEL_CFG_7 7
#define INST_SEL_CFG_0 0
#define INST_SEL_CFG_1 1
#define INST_SEL_CFG_2 2
#define INST_SEL_CFG_3 3
#define INST_SEL_CFG_4 4
#define INST_SEL_CFG_5 5
#define INST_SEL_CFG_6 6
#define INST_SEL_CFG_7 7
#define SYS_SGL_FIFO_EMPTY 0
#define SYS_SGL_FIFO_FULL 1
#define TS_ENABLE_0 0
#define TS_ENABLE_1 1
#define TS_ENABLE_2 2
#define RAM_CONFLICT_ALM_0 0
#define RAM_CONFLICT_ALM_1 1
#define RAM_CONFLICT_ALM_2 2
#define RAM_CONFLICT_ALM_3 3
#define CFG_PLS 0

/******************************************************************************
*
* MOUDLE_NAME
*
*	 flexe_demux
*
* MOUDEL_ADDRESS
*
*	 flexe_demux
*
* MOUDLE_OFFSET
*
*    flexe_demux
*
* MOUDLE_RegisterName
*
*	 flexe_demux
*
*
******************************************************************************/
/************flexe_demux*****************/
#define READY_TS_0 0
#define READY_TS_1 1
#define READY_TS_2 2
#define FLEXEGROUP_INFO 0
#define GROUP_CFG_0 0
#define GROUP_CFG_1 1
#define REORDER_ASIC_0 0
#define REORDER_ASIC_1 1
#define AUTOSET_EN 0
#define MODE_INFO 0
#define REORDER_INFO 0
#define CALENDAR_CFG_0 0
#define CHANNEL_OWN_TSGROUP 0
#define MAXLEVEL_CLR 0
#define READY_TS_B_0 0
#define READY_TS_B_1 1
#define READY_TS_B_2 2
#define CALENDAR_CFG_B_0 0
#define CHANNEL_OWN_TSGROUP_B_0 0
#define ENABLE_AB 0
#define VERIFY_CFG_0 0
#define VERIFY_CFG_1 1
#define LOCALFAULT_EN 0
#define THRESHOLD 0
#define THRESHOLD_FULL_0 0
#define THRESHOLD_FULL_1 1
#define THRESHOLD_EMPTY_0 0
#define THRESHOLD_EMPTY_1 1
#define MAXLEVEL_INTERLEAVE_0 0
#define MAXLEVEL_INTERLEAVE_1 1
#define INTERLEAVE_RST 0
#define FLEXEDEMUX_RAM_FULL_0 0
#define FLEXEDEMUX_RAM_FULL_1 1
#define FLEXEDEMUX_RAM_FULL_2 2
#define FLEXEDEMUX_RAM_FULL_3 3
#define FLEXEDEMUX_FIFO_FULL 0
#define FLEXEDEMUX_FIFO_EMPTY 0
#define FP_CHECK_ALM 0
#define HEAD_CHECK_PHY_ALM 0
#define HEAD_CHECK_CHANNEL_ALM_0 0
#define HEAD_CHECK_CHANNEL_ALM_1 1
#define HEAD_CHECK_CHANNEL_ALM_2 2

/******************************************************************************
*
* MOUDLE_NAME
*
*	 rateadp
*
* MOUDEL_ADDRESS
*
*	 rateadp
*
* MOUDLE_OFFSET
*
*    rateadp
*
* MOUDLE_RegisterName
*
*	 rateadp
*
*
******************************************************************************/
/**************rateadp***********/
#define LOW_LVL 0
#define STA_LVL 1
#define HIG_LVL 2
#define RSK_LVL 3
#define RATEADP_FULL_0 0
#define RATEADP_FULL_1 1
#define RATEADP_FULL_2 2
#define RATEADP_EMPTY_0 0
#define RATEADP_EMPTY_1 1
#define RATEADP_EMPTY_2 2
#define RATEADP_OVERFLOW_0 0
#define RATEADP_OVERFLOW_1 1
#define RATEADP_OVERFLOW_2 2
#define BLOCK_TYPE 0
#define RATEADP_IND_SEL 0
#define RATEADP_INS_GAP 0
#define RATEADP_DEL_GAP 1
#define RATEADP_CHID_LVL_HIST 0
#define RATEADP_CLR_HIST 1
#define RATEADP_LOW_HIST 0
#define RATEADP_HIG_HIST 1
#define RATEADP_BLK_NUM 0
#define CHAN_EN 1
#define MAP_SEQ 0
#define MAP_CHID 1
#define RATEADP_MAP_EN 2

/******************************************************************************
*
* MOUDLE_NAME
*
*	 adj
*
* MOUDEL_ADDRESS
*
*	 adj
*
* MOUDLE_OFFSET
*
*    adj
*
* MOUDLE_RegisterName
*
*	 adj
*
*
******************************************************************************/
/**************adj***********/
#define I_NOMINAL_VAL 0
#define I_ADJ_P_I 1
#define I_ADJ_P_N 2
#define I_ADJ_P_D 3
#define I_ADJ_ONLY 4
#define I_FIFO_RESTART_EN 5
#define I_ADJ_ALM_RESTART_EN 6
#define I_PLL_COEFF 7
#define I_ADJ_P_D_EN 8
#define I_CFG_FINISH 9
#define O_ADJ_CNT_ALM_0 0
#define O_ADJ_CNT_ALM_1 1
#define O_ADJ_CNT_ALM_2 2
#define O_ADJ_CORE_ALM_0 0
#define O_ADJ_CORE_ALM_1 1
#define O_ADJ_CORE_ALM_2 2

/******************************************************************************
*
* MOUDLE_NAME
*
*	 mac_rateadpt
*
* MOUDEL_ADDRESS
*
*	 mac_rateadpt
*
* MOUDLE_OFFSET
*
*    mac_rateadpt
*
* MOUDLE_RegisterName
*
*	 mac_rateadpt
*
*
******************************************************************************/
/***************mac_rateadpt************/
#define MAC_LOW_LVL 0
#define MAC_STA_LVL 1
#define MAC_HIG_LVL 2
#define MAC_RSK_LVL 3
#define MAC_RATEADP_FULL_0 0
#define MAC_RATEADP_FULL_1 1
#define MAC_RATEADP_FULL_2 2
#define MAC_RATEADP_EMPTY_0 0
#define MAC_RATEADP_EMPTY_1 1
#define MAC_RATEADP_EMPTY_2 2
#define OVERFLOW_0 0
#define OVERFLOW_1 1
#define OVERFLOW_2 2
#define DEL_MANY_EN_0 0
#define DEL_MANY_EN_1 1
#define DEL_MANY_EN_2 2
#define MAC_RATEADP_CHID_LVL_HIST 0
#define MAC_RATEADP_CLR_HIST 1
#define IND_SEL 0
#define INS_GAP 0
#define DEL_GAP 1
#define MAC_RATEADP_LOW_HIST 0
#define MAC_RATEADP_HIG_HIST 1
#define MAC_RATEADP_BLK_NUM 0
#define MAC_CHAN_EN 1
#define MAC_MAP_SEQ 0
#define MAC_MAP_CHID 1
#define MAC_RATEADP_MAP_EN 2

/******************************************************************************
*
* MOUDLE_NAME
*
*	 mac_adj
*
* MOUDEL_ADDRESS
*
*	 mac_adj
*
* MOUDLE_OFFSET
*
*    mac_adj
*
* MOUDLE_RegisterName
*
*	 mac_adj
*
*
******************************************************************************/
/**************mac_adj***********/
#define MAC_I_NOMINAL_VAL 0
#define MAC_I_ADJ_P_I 1
#define MAC_I_ADJ_P_N 2
#define MAC_I_ADJ_P_D 3
#define MAC_I_ADJ_ONLY 4
#define MAC_I_FIFO_RESTART_EN 5
#define MAC_I_ADJ_ALM_RESTART_EN 6
#define MAC_I_PLL_COEFF 7
#define MAC_I_ADJ_P_D_EN 8
#define MAC_I_CFG_FINISH 9
#define MAC_O_ADJ_CNT_ALM_0 0
#define MAC_O_ADJ_CNT_ALM_1 1
#define MAC_O_ADJ_CNT_ALM_2 2
#define MAC_O_ADJ_CORE_ALM_0 0
#define MAC_O_ADJ_CORE_ALM_1 1
#define MAC_O_ADJ_CORE_ALM_2 2

/******************************************************************************
*
* MOUDLE_NAME
*
*	 oam_rx
*
* MOUDEL_ADDRESS
*
*	 oam_rx
*
* MOUDLE_OFFSET
*
*    oam_rx
*
* MOUDLE_RegisterName
*
*	 oam_rx
*
*
******************************************************************************/
/*****************oam_rx****************/
#define RX_GLB_BIPERR_BIT_CFG 0
#define RX_GLB_BEIERR_BIT_CFG 1
#define CRC_INVERT 2
#define BYPASS_OAM_RX 3
#define OAM_RX_RDI_BLK 4
#define OAM_IDFY_37_34_CFG 5
#define BAS_POS_CNT 6
#define CRC_BASE_CNT_EN 7
#define CRC_OAM_CAL_EN 8
#define BYPASS_OAM_ALM_ALL 9
#define OAM_RX_BASE_LOS_SEL 10
#define OAM_CNT_CLR_NUM 11
#define BASE_OAM_CNT_CLR_NUM 12
#define RX_BIPERR_CNT_CLR_0 0
#define RX_BIPERR_CNT_CLR_1 1
#define RX_BEIERR_CNT_CLR_0 0
#define RX_BEIERR_CNT_CLR_1 1
#define RX_SDBIP_0 0
#define RX_SDBIP_1 1
#define RX_IDLE_RPL_EN_0 0
#define RX_IDLE_RPL_EN_1 1
#define BYPASS_CHNL_0 0
#define BYPASS_CHNL_1 1
#define PERIOD_CFG_0 0
#define PERIOD_CFG_1 1
#define PERIOD_CFG_2 2
#define PERIOD_SEL_0 0
#define PERIOD_SEL_1 1
#define PERIOD_SEL_2 2
#define OAM_CNT_CLR 0
#define OAM_CNT_ALL_CLR 1
#define OAMRX_TS_RAM_INIT 2
#define BASE_OAM_CNT_CLR 3
#define BASE_OAM_CNT_ALL_CLR 4
#define BASE_OAM_RAM_CLR 5
#define FIFO_CNT_CLR 6
#define OAM_CNT_CLR_DONE 0
#define OAMRX_TS_RAM_INIT_DONE 1
#define BASE_OAM_CNT_CLR_DONE 2
#define FIFO_CNT_MAX 3
#define BASE_LOS_ALM_EN 0
#define BAS_CS_LF_ALM_EN 1
#define BAS_CS_RF_ALM_EN 2
#define BAS_CSF_LPI_ALM_EN 3
#define SDBIP_ALM_EN 4
#define SDBEI_ALM_EN 5
#define CLIENT_LF_ALM_EN 6
#define CLIENT_RF_ALM_EN 7
#define CLIENT_LPI_ALM_EN 8
#define BAS_RDI_ALM_EN 9
#define CLIENT_FAIL_ALM_EN 10
#define TX_SDBIP_ALM_EN 11
#define TX_SDBEI_ALM_EN 12
#define TX_BASE_LOS_ALM_EN 13
#define TX_BAS_PERIOD_ALM_EN 14
#define TX_CLIENT_LF_ALM_EN 15
#define TX_CLIENT_RF_ALM_EN 16
#define TX_CLIENT_LPI_ALM_EN 17
#define RX_LPI_ALM_EN 18
#define TX_SFBIP_ALM_EN 19
#define TX_SFBEI_ALM_EN 20
#define SFBIP_ALM_EN 21
#define SFBEI_ALM_EN 22
#define BAS_PERIOD_ALM_EN 23
#define BASE_PERIOD_0 0
#define BASE_PERIOD_1 1
#define BASE_PERIOD_2 2
#define BASE_PERIOD_3 3
#define BASE_PERIOD_4 4
#define DEMUX_CLIENT_ALM_0 0
#define DEMUX_CLIENT_ALM_1 1
#define DEMUX_CLIENT_ALM_2 2
#define OAM_RX_RDI 0
#define OAM_RX_FULL_1 1
#define ECC_ERR_CORRECT 2
#define ECC_ERR_UNCORRECT 3
#define RX_SFBIP_0 0
#define RX_SFBIP_1 1
#define RX_SFBIP_2 2
#define RX_SFBEI_0 0
#define RX_SFBEI_1 1
#define RX_SFBEI_2 2
#define CRC_ALM_OUT_0 0
#define CRC_ALM_OUT_1 1
#define BAS_RDI_0 0
#define BAS_RDI_1 1
#define BAS_CSF_LPI_0 0
#define BAS_CSF_LPI_1 1
#define BAS_CS_LF_0 0
#define BAS_CS_LF_1 1
#define BAS_CS_RF_0 0
#define BAS_CS_RF_1 1
#define BAS_CRC_ERR_0 0
#define BAS_CRC_ERR_1 1
#define BAS_NO_RECEIVE_ALM_0 0
#define BAS_NO_RECEIVE_ALM_1 1
#define RDI_LF_0 0
#define RDI_LF_1 1
#define RX_SDBEI_0 0
#define RX_SDBEI_1 1
#define RF_ALM_0 0
#define RF_ALM_1 1
#define LPI_ALM_0 0
#define LPI_ALM_1 1
#define RX_BIPERR_CNT_CLR_2 2
#define RX_BEIERR_CNT_CLR_2 2
#define RX_SDBIP_2 2
#define RX_IDLE_RPL_EN_2 2
#define BYPASS_CHNL_2 2
#define PERIOD_CFG_3 3
#define PERIOD_CFG_4 4
#define PERIOD_SEL_3 3
#define PERIOD_SEL_4 4
#define CRC_ALM_OUT_2 2
#define BAS_RDI_2 2
#define BAS_CSF_LPI_2 2
#define BAS_CS_LF_2 2
#define BAS_CS_RF_2 2
#define BAS_CRC_ERR_2 2
#define BAS_NO_RECEIVE_ALM_2 2
#define RDI_LF_2 2
#define RX_SDBEI_2 2
#define RF_ALM_2 2
#define LPI_ALM_2 2
#define BYPASS_XGMII_0 0
#define BYPASS_XGMII_1 1
#define BYPASS_XGMII_2 2
#define RX_LPI_RPL_EN_0 0
#define RX_LPI_RPL_EN_1 1
#define RX_LPI_RPL_EN_2 2
#define BAS_PERIOD_0 0
#define BAS_PERIOD_1 1
#define BAS_PERIOD_2 0
#define BAS_PERIOD_3 1
#define BYPASS_OAM_ALM_CH_0 0
#define BYPASS_OAM_ALM_CH_1 1
#define BYPASS_OAM_ALM_CH_2 2
#define BAS_RAM_CLR_CH 0
#define TIMESTAMP_SEL 1
#define OAM_RX_LPI_PRE_CNT 2
#define OAM_RX_LPI_POS_CNT 3
#define BASE_LOS_RPL_EN 4
#define BIP_RECAL_EN 5
#define BASE_BIP_CRC_EN 6
#define OAM_TYPE_BIP_CAL_EN_0 0
#define OAM_TYPE_BIP_CAL_EN_1 1
#define OAM_TYPE_BIP_CAL_EN_2 2
#define LF_MANUAL_INSERT_0 0
#define LF_MANUAL_INSERT_1 1
#define LF_MANUAL_INSERT_2 2
#define RF_MANUAL_INSERT_0 0
#define RF_MANUAL_INSERT_1 1
#define RF_MANUAL_INSERT_2 2
#define LPI_MANUAL_INSERT_0 0
#define LPI_MANUAL_INSERT_1 1
#define LPI_MANUAL_INSERT_2 2
#define BAS_PERIOD_ALM_0 0
#define BAS_PERIOD_ALM_1 1
#define BAS_PERIOD_ALM_2 2
#define RX_SDBIP_0_INT 0
#define RX_SDBIP_1_INT 1
#define RX_SDBIP_2_INT 2
#define RX_SDBIP_3_INT 3
#define RX_SDBIP_4_INT 4
#define RX_SDBIP_5_INT 5
#define RX_SDBIP_6_INT 6
#define RX_SDBIP_7_INT 7
#define RX_SDBIP_8_INT 8
#define RX_SDBIP_9_INT 9
#define RX_SDBIP_10_INT 10
#define RX_SDBIP_11_INT 11
#define RX_SDBIP_12_INT 12
#define RX_SDBIP_13_INT 13
#define RX_SDBIP_14_INT 14
#define RX_SDBIP_15_INT 15
#define RX_SDBIP_0_INT_MASK 0
#define RX_SDBIP_1_INT_MASK 1
#define RX_SDBIP_2_INT_MASK 2
#define RX_SDBIP_3_INT_MASK 3
#define RX_SDBIP_4_INT_MASK 4
#define RX_SDBIP_5_INT_MASK 5
#define RX_SDBIP_6_INT_MASK 6
#define RX_SDBIP_7_INT_MASK 7
#define RX_SDBIP_8_INT_MASK 8
#define RX_SDBIP_9_INT_MASK 9
#define RX_SDBIP_10_INT_MASK 10
#define RX_SDBIP_11_INT_MASK 11
#define RX_SDBIP_12_INT_MASK 12
#define RX_SDBIP_13_INT_MASK 13
#define RX_SDBIP_14_INT_MASK 14
#define RX_SDBIP_15_INT_MASK 15
#define RX_SDBIP_16_INT 0
#define RX_SDBIP_17_INT 1
#define RX_SDBIP_18_INT 2
#define RX_SDBIP_19_INT 3
#define RX_SDBIP_20_INT 4
#define RX_SDBIP_21_INT 5
#define RX_SDBIP_22_INT 6
#define RX_SDBIP_23_INT 7
#define RX_SDBIP_24_INT 8
#define RX_SDBIP_25_INT 9
#define RX_SDBIP_26_INT 10
#define RX_SDBIP_27_INT 11
#define RX_SDBIP_28_INT 12
#define RX_SDBIP_29_INT 13
#define RX_SDBIP_30_INT 14
#define RX_SDBIP_31_INT 15
#define RX_SDBIP_16_INT_MASK 0
#define RX_SDBIP_17_INT_MASK 1
#define RX_SDBIP_18_INT_MASK 2
#define RX_SDBIP_19_INT_MASK 3
#define RX_SDBIP_20_INT_MASK 4
#define RX_SDBIP_21_INT_MASK 5
#define RX_SDBIP_22_INT_MASK 6
#define RX_SDBIP_23_INT_MASK 7
#define RX_SDBIP_24_INT_MASK 8
#define RX_SDBIP_25_INT_MASK 9
#define RX_SDBIP_26_INT_MASK 10
#define RX_SDBIP_27_INT_MASK 11
#define RX_SDBIP_28_INT_MASK 12
#define RX_SDBIP_29_INT_MASK 13
#define RX_SDBIP_30_INT_MASK 14
#define RX_SDBIP_31_INT_MASK 15
#define RX_SDBIP_32_INT 0
#define RX_SDBIP_33_INT 1
#define RX_SDBIP_34_INT 2
#define RX_SDBIP_35_INT 3
#define RX_SDBIP_36_INT 4
#define RX_SDBIP_37_INT 5
#define RX_SDBIP_38_INT 6
#define RX_SDBIP_39_INT 7
#define RX_SDBIP_40_INT 8
#define RX_SDBIP_41_INT 9
#define RX_SDBIP_42_INT 10
#define RX_SDBIP_43_INT 11
#define RX_SDBIP_44_INT 12
#define RX_SDBIP_45_INT 13
#define RX_SDBIP_46_INT 14
#define RX_SDBIP_47_INT 15
#define RX_SDBIP_32_INT_MASK 0
#define RX_SDBIP_33_INT_MASK 1
#define RX_SDBIP_34_INT_MASK 2
#define RX_SDBIP_35_INT_MASK 3
#define RX_SDBIP_36_INT_MASK 4
#define RX_SDBIP_37_INT_MASK 5
#define RX_SDBIP_38_INT_MASK 6
#define RX_SDBIP_39_INT_MASK 7
#define RX_SDBIP_40_INT_MASK 8
#define RX_SDBIP_41_INT_MASK 9
#define RX_SDBIP_42_INT_MASK 10
#define RX_SDBIP_43_INT_MASK 11
#define RX_SDBIP_44_INT_MASK 12
#define RX_SDBIP_45_INT_MASK 13
#define RX_SDBIP_46_INT_MASK 14
#define RX_SDBIP_47_INT_MASK 15
#define RX_SDBIP_48_INT 0
#define RX_SDBIP_49_INT 1
#define RX_SDBIP_50_INT 2
#define RX_SDBIP_51_INT 3
#define RX_SDBIP_52_INT 4
#define RX_SDBIP_53_INT 5
#define RX_SDBIP_54_INT 6
#define RX_SDBIP_55_INT 7
#define RX_SDBIP_56_INT 8
#define RX_SDBIP_57_INT 9
#define RX_SDBIP_58_INT 10
#define RX_SDBIP_59_INT 11
#define RX_SDBIP_60_INT 12
#define RX_SDBIP_61_INT 13
#define RX_SDBIP_62_INT 14
#define RX_SDBIP_63_INT 15
#define RX_SDBIP_48_INT_MASK 0
#define RX_SDBIP_49_INT_MASK 1
#define RX_SDBIP_50_INT_MASK 2
#define RX_SDBIP_51_INT_MASK 3
#define RX_SDBIP_52_INT_MASK 4
#define RX_SDBIP_53_INT_MASK 5
#define RX_SDBIP_54_INT_MASK 6
#define RX_SDBIP_55_INT_MASK 7
#define RX_SDBIP_56_INT_MASK 8
#define RX_SDBIP_57_INT_MASK 9
#define RX_SDBIP_58_INT_MASK 10
#define RX_SDBIP_59_INT_MASK 11
#define RX_SDBIP_60_INT_MASK 12
#define RX_SDBIP_61_INT_MASK 13
#define RX_SDBIP_62_INT_MASK 14
#define RX_SDBIP_63_INT_MASK 15
#define RX_SDBIP_64_INT 0
#define RX_SDBIP_65_INT 1
#define RX_SDBIP_66_INT 2
#define RX_SDBIP_67_INT 3
#define RX_SDBIP_68_INT 4
#define RX_SDBIP_69_INT 5
#define RX_SDBIP_70_INT 6
#define RX_SDBIP_71_INT 7
#define RX_SDBIP_72_INT 8
#define RX_SDBIP_73_INT 9
#define RX_SDBIP_74_INT 10
#define RX_SDBIP_75_INT 11
#define RX_SDBIP_76_INT 12
#define RX_SDBIP_77_INT 13
#define RX_SDBIP_78_INT 14
#define RX_SDBIP_79_INT 15
#define RX_SDBIP_64_INT_MASK 0
#define RX_SDBIP_65_INT_MASK 1
#define RX_SDBIP_66_INT_MASK 2
#define RX_SDBIP_67_INT_MASK 3
#define RX_SDBIP_68_INT_MASK 4
#define RX_SDBIP_69_INT_MASK 5
#define RX_SDBIP_70_INT_MASK 6
#define RX_SDBIP_71_INT_MASK 7
#define RX_SDBIP_72_INT_MASK 8
#define RX_SDBIP_73_INT_MASK 9
#define RX_SDBIP_74_INT_MASK 10
#define RX_SDBIP_75_INT_MASK 11
#define RX_SDBIP_76_INT_MASK 12
#define RX_SDBIP_77_INT_MASK 13
#define RX_SDBIP_78_INT_MASK 14
#define RX_SDBIP_79_INT_MASK 15
#define BAS_RDI_0_INT 0
#define BAS_RDI_1_INT 1
#define BAS_RDI_2_INT 2
#define BAS_RDI_3_INT 3
#define BAS_RDI_4_INT 4
#define BAS_RDI_5_INT 5
#define BAS_RDI_6_INT 6
#define BAS_RDI_7_INT 7
#define BAS_RDI_8_INT 8
#define BAS_RDI_9_INT 9
#define BAS_RDI_10_INT 10
#define BAS_RDI_11_INT 11
#define BAS_RDI_12_INT 12
#define BAS_RDI_13_INT 13
#define BAS_RDI_14_INT 14
#define BAS_RDI_15_INT 15
#define BAS_RDI_0_INT_MASK 0
#define BAS_RDI_1_INT_MASK 1
#define BAS_RDI_2_INT_MASK 2
#define BAS_RDI_3_INT_MASK 3
#define BAS_RDI_4_INT_MASK 4
#define BAS_RDI_5_INT_MASK 5
#define BAS_RDI_6_INT_MASK 6
#define BAS_RDI_7_INT_MASK 7
#define BAS_RDI_8_INT_MASK 8
#define BAS_RDI_9_INT_MASK 9
#define BAS_RDI_10_INT_MASK 10
#define BAS_RDI_11_INT_MASK 11
#define BAS_RDI_12_INT_MASK 12
#define BAS_RDI_13_INT_MASK 13
#define BAS_RDI_14_INT_MASK 14
#define BAS_RDI_15_INT_MASK 15
#define BAS_RDI_16_INT 0
#define BAS_RDI_17_INT 1
#define BAS_RDI_18_INT 2
#define BAS_RDI_19_INT 3
#define BAS_RDI_20_INT 4
#define BAS_RDI_21_INT 5
#define BAS_RDI_22_INT 6
#define BAS_RDI_23_INT 7
#define BAS_RDI_24_INT 8
#define BAS_RDI_25_INT 9
#define BAS_RDI_26_INT 10
#define BAS_RDI_27_INT 11
#define BAS_RDI_28_INT 12
#define BAS_RDI_29_INT 13
#define BAS_RDI_30_INT 14
#define BAS_RDI_31_INT 15
#define BAS_RDI_16_INT_MASK 0
#define BAS_RDI_17_INT_MASK 1
#define BAS_RDI_18_INT_MASK 2
#define BAS_RDI_19_INT_MASK 3
#define BAS_RDI_20_INT_MASK 4
#define BAS_RDI_21_INT_MASK 5
#define BAS_RDI_22_INT_MASK 6
#define BAS_RDI_23_INT_MASK 7
#define BAS_RDI_24_INT_MASK 8
#define BAS_RDI_25_INT_MASK 9
#define BAS_RDI_26_INT_MASK 10
#define BAS_RDI_27_INT_MASK 11
#define BAS_RDI_28_INT_MASK 12
#define BAS_RDI_29_INT_MASK 13
#define BAS_RDI_30_INT_MASK 14
#define BAS_RDI_31_INT_MASK 15
#define BAS_RDI_32_INT 0
#define BAS_RDI_33_INT 1
#define BAS_RDI_34_INT 2
#define BAS_RDI_35_INT 3
#define BAS_RDI_36_INT 4
#define BAS_RDI_37_INT 5
#define BAS_RDI_38_INT 6
#define BAS_RDI_39_INT 7
#define BAS_RDI_40_INT 8
#define BAS_RDI_41_INT 9
#define BAS_RDI_42_INT 10
#define BAS_RDI_43_INT 11
#define BAS_RDI_44_INT 12
#define BAS_RDI_45_INT 13
#define BAS_RDI_46_INT 14
#define BAS_RDI_47_INT 15
#define BAS_RDI_32_INT_MASK 0
#define BAS_RDI_33_INT_MASK 1
#define BAS_RDI_34_INT_MASK 2
#define BAS_RDI_35_INT_MASK 3
#define BAS_RDI_36_INT_MASK 4
#define BAS_RDI_37_INT_MASK 5
#define BAS_RDI_38_INT_MASK 6
#define BAS_RDI_39_INT_MASK 7
#define BAS_RDI_40_INT_MASK 8
#define BAS_RDI_41_INT_MASK 9
#define BAS_RDI_42_INT_MASK 10
#define BAS_RDI_43_INT_MASK 11
#define BAS_RDI_44_INT_MASK 12
#define BAS_RDI_45_INT_MASK 13
#define BAS_RDI_46_INT_MASK 14
#define BAS_RDI_47_INT_MASK 15
#define BAS_RDI_48_INT 0
#define BAS_RDI_49_INT 1
#define BAS_RDI_50_INT 2
#define BAS_RDI_51_INT 3
#define BAS_RDI_52_INT 4
#define BAS_RDI_53_INT 5
#define BAS_RDI_54_INT 6
#define BAS_RDI_55_INT 7
#define BAS_RDI_56_INT 8
#define BAS_RDI_57_INT 9
#define BAS_RDI_58_INT 10
#define BAS_RDI_59_INT 11
#define BAS_RDI_60_INT 12
#define BAS_RDI_61_INT 13
#define BAS_RDI_62_INT 14
#define BAS_RDI_63_INT 15
#define BAS_RDI_48_INT_MASK 0
#define BAS_RDI_49_INT_MASK 1
#define BAS_RDI_50_INT_MASK 2
#define BAS_RDI_51_INT_MASK 3
#define BAS_RDI_52_INT_MASK 4
#define BAS_RDI_53_INT_MASK 5
#define BAS_RDI_54_INT_MASK 6
#define BAS_RDI_55_INT_MASK 7
#define BAS_RDI_56_INT_MASK 8
#define BAS_RDI_57_INT_MASK 9
#define BAS_RDI_58_INT_MASK 10
#define BAS_RDI_59_INT_MASK 11
#define BAS_RDI_60_INT_MASK 12
#define BAS_RDI_61_INT_MASK 13
#define BAS_RDI_62_INT_MASK 14
#define BAS_RDI_63_INT_MASK 15
#define BAS_RDI_64_INT 0
#define BAS_RDI_65_INT 1
#define BAS_RDI_66_INT 2
#define BAS_RDI_67_INT 3
#define BAS_RDI_68_INT 4
#define BAS_RDI_69_INT 5
#define BAS_RDI_70_INT 6
#define BAS_RDI_71_INT 7
#define BAS_RDI_72_INT 8
#define BAS_RDI_73_INT 9
#define BAS_RDI_74_INT 10
#define BAS_RDI_75_INT 11
#define BAS_RDI_76_INT 12
#define BAS_RDI_77_INT 13
#define BAS_RDI_78_INT 14
#define BAS_RDI_79_INT 15
#define BAS_RDI_64_INT_MASK 0
#define BAS_RDI_65_INT_MASK 1
#define BAS_RDI_66_INT_MASK 2
#define BAS_RDI_67_INT_MASK 3
#define BAS_RDI_68_INT_MASK 4
#define BAS_RDI_69_INT_MASK 5
#define BAS_RDI_70_INT_MASK 6
#define BAS_RDI_71_INT_MASK 7
#define BAS_RDI_72_INT_MASK 8
#define BAS_RDI_73_INT_MASK 9
#define BAS_RDI_74_INT_MASK 10
#define BAS_RDI_75_INT_MASK 11
#define BAS_RDI_76_INT_MASK 12
#define BAS_RDI_77_INT_MASK 13
#define BAS_RDI_78_INT_MASK 14
#define BAS_RDI_79_INT_MASK 15
#define BAS_CSF_LPI_0_INT 0
#define BAS_CSF_LPI_1_INT 1
#define BAS_CSF_LPI_2_INT 2
#define BAS_CSF_LPI_3_INT 3
#define BAS_CSF_LPI_4_INT 4
#define BAS_CSF_LPI_5_INT 5
#define BAS_CSF_LPI_6_INT 6
#define BAS_CSF_LPI_7_INT 7
#define BAS_CSF_LPI_8_INT 8
#define BAS_CSF_LPI_9_INT 9
#define BAS_CSF_LPI_10_INT 10
#define BAS_CSF_LPI_11_INT 11
#define BAS_CSF_LPI_12_INT 12
#define BAS_CSF_LPI_13_INT 13
#define BAS_CSF_LPI_14_INT 14
#define BAS_CSF_LPI_15_INT 15
#define BAS_CSF_LPI_0_INT_MASK 0
#define BAS_CSF_LPI_1_INT_MASK 1
#define BAS_CSF_LPI_2_INT_MASK 2
#define BAS_CSF_LPI_3_INT_MASK 3
#define BAS_CSF_LPI_4_INT_MASK 4
#define BAS_CSF_LPI_5_INT_MASK 5
#define BAS_CSF_LPI_6_INT_MASK 6
#define BAS_CSF_LPI_7_INT_MASK 7
#define BAS_CSF_LPI_8_INT_MASK 8
#define BAS_CSF_LPI_9_INT_MASK 9
#define BAS_CSF_LPI_10_INT_MASK 10
#define BAS_CSF_LPI_11_INT_MASK 11
#define BAS_CSF_LPI_12_INT_MASK 12
#define BAS_CSF_LPI_13_INT_MASK 13
#define BAS_CSF_LPI_14_INT_MASK 14
#define BAS_CSF_LPI_15_INT_MASK 15
#define BAS_CSF_LPI_16_INT 0
#define BAS_CSF_LPI_17_INT 1
#define BAS_CSF_LPI_18_INT 2
#define BAS_CSF_LPI_19_INT 3
#define BAS_CSF_LPI_20_INT 4
#define BAS_CSF_LPI_21_INT 5
#define BAS_CSF_LPI_22_INT 6
#define BAS_CSF_LPI_23_INT 7
#define BAS_CSF_LPI_24_INT 8
#define BAS_CSF_LPI_25_INT 9
#define BAS_CSF_LPI_26_INT 10
#define BAS_CSF_LPI_27_INT 11
#define BAS_CSF_LPI_28_INT 12
#define BAS_CSF_LPI_29_INT 13
#define BAS_CSF_LPI_30_INT 14
#define BAS_CSF_LPI_31_INT 15
#define BAS_CSF_LPI_16_INT_MASK 0
#define BAS_CSF_LPI_17_INT_MASK 1
#define BAS_CSF_LPI_18_INT_MASK 2
#define BAS_CSF_LPI_19_INT_MASK 3
#define BAS_CSF_LPI_20_INT_MASK 4
#define BAS_CSF_LPI_21_INT_MASK 5
#define BAS_CSF_LPI_22_INT_MASK 6
#define BAS_CSF_LPI_23_INT_MASK 7
#define BAS_CSF_LPI_24_INT_MASK 8
#define BAS_CSF_LPI_25_INT_MASK 9
#define BAS_CSF_LPI_26_INT_MASK 10
#define BAS_CSF_LPI_27_INT_MASK 11
#define BAS_CSF_LPI_28_INT_MASK 12
#define BAS_CSF_LPI_29_INT_MASK 13
#define BAS_CSF_LPI_30_INT_MASK 14
#define BAS_CSF_LPI_31_INT_MASK 15
#define BAS_CSF_LPI_32_INT 0
#define BAS_CSF_LPI_33_INT 1
#define BAS_CSF_LPI_34_INT 2
#define BAS_CSF_LPI_35_INT 3
#define BAS_CSF_LPI_36_INT 4
#define BAS_CSF_LPI_37_INT 5
#define BAS_CSF_LPI_38_INT 6
#define BAS_CSF_LPI_39_INT 7
#define BAS_CSF_LPI_40_INT 8
#define BAS_CSF_LPI_41_INT 9
#define BAS_CSF_LPI_42_INT 10
#define BAS_CSF_LPI_43_INT 11
#define BAS_CSF_LPI_44_INT 12
#define BAS_CSF_LPI_45_INT 13
#define BAS_CSF_LPI_46_INT 14
#define BAS_CSF_LPI_47_INT 15
#define BAS_CSF_LPI_32_INT_MASK 0
#define BAS_CSF_LPI_33_INT_MASK 1
#define BAS_CSF_LPI_34_INT_MASK 2
#define BAS_CSF_LPI_35_INT_MASK 3
#define BAS_CSF_LPI_36_INT_MASK 4
#define BAS_CSF_LPI_37_INT_MASK 5
#define BAS_CSF_LPI_38_INT_MASK 6
#define BAS_CSF_LPI_39_INT_MASK 7
#define BAS_CSF_LPI_40_INT_MASK 8
#define BAS_CSF_LPI_41_INT_MASK 9
#define BAS_CSF_LPI_42_INT_MASK 10
#define BAS_CSF_LPI_43_INT_MASK 11
#define BAS_CSF_LPI_44_INT_MASK 12
#define BAS_CSF_LPI_45_INT_MASK 13
#define BAS_CSF_LPI_46_INT_MASK 14
#define BAS_CSF_LPI_47_INT_MASK 15
#define BAS_CSF_LPI_48_INT 0
#define BAS_CSF_LPI_49_INT 1
#define BAS_CSF_LPI_50_INT 2
#define BAS_CSF_LPI_51_INT 3
#define BAS_CSF_LPI_52_INT 4
#define BAS_CSF_LPI_53_INT 5
#define BAS_CSF_LPI_54_INT 6
#define BAS_CSF_LPI_55_INT 7
#define BAS_CSF_LPI_56_INT 8
#define BAS_CSF_LPI_57_INT 9
#define BAS_CSF_LPI_58_INT 10
#define BAS_CSF_LPI_59_INT 11
#define BAS_CSF_LPI_60_INT 12
#define BAS_CSF_LPI_61_INT 13
#define BAS_CSF_LPI_62_INT 14
#define BAS_CSF_LPI_63_INT 15
#define BAS_CSF_LPI_48_INT_MASK 0
#define BAS_CSF_LPI_49_INT_MASK 1
#define BAS_CSF_LPI_50_INT_MASK 2
#define BAS_CSF_LPI_51_INT_MASK 3
#define BAS_CSF_LPI_52_INT_MASK 4
#define BAS_CSF_LPI_53_INT_MASK 5
#define BAS_CSF_LPI_54_INT_MASK 6
#define BAS_CSF_LPI_55_INT_MASK 7
#define BAS_CSF_LPI_56_INT_MASK 8
#define BAS_CSF_LPI_57_INT_MASK 9
#define BAS_CSF_LPI_58_INT_MASK 10
#define BAS_CSF_LPI_59_INT_MASK 11
#define BAS_CSF_LPI_60_INT_MASK 12
#define BAS_CSF_LPI_61_INT_MASK 13
#define BAS_CSF_LPI_62_INT_MASK 14
#define BAS_CSF_LPI_63_INT_MASK 15
#define BAS_CSF_LPI_64_INT 0
#define BAS_CSF_LPI_65_INT 1
#define BAS_CSF_LPI_66_INT 2
#define BAS_CSF_LPI_67_INT 3
#define BAS_CSF_LPI_68_INT 4
#define BAS_CSF_LPI_69_INT 5
#define BAS_CSF_LPI_70_INT 6
#define BAS_CSF_LPI_71_INT 7
#define BAS_CSF_LPI_72_INT 8
#define BAS_CSF_LPI_73_INT 9
#define BAS_CSF_LPI_74_INT 10
#define BAS_CSF_LPI_75_INT 11
#define BAS_CSF_LPI_76_INT 12
#define BAS_CSF_LPI_77_INT 13
#define BAS_CSF_LPI_78_INT 14
#define BAS_CSF_LPI_79_INT 15
#define BAS_CSF_LPI_64_INT_MASK 0
#define BAS_CSF_LPI_65_INT_MASK 1
#define BAS_CSF_LPI_66_INT_MASK 2
#define BAS_CSF_LPI_67_INT_MASK 3
#define BAS_CSF_LPI_68_INT_MASK 4
#define BAS_CSF_LPI_69_INT_MASK 5
#define BAS_CSF_LPI_70_INT_MASK 6
#define BAS_CSF_LPI_71_INT_MASK 7
#define BAS_CSF_LPI_72_INT_MASK 8
#define BAS_CSF_LPI_73_INT_MASK 9
#define BAS_CSF_LPI_74_INT_MASK 10
#define BAS_CSF_LPI_75_INT_MASK 11
#define BAS_CSF_LPI_76_INT_MASK 12
#define BAS_CSF_LPI_77_INT_MASK 13
#define BAS_CSF_LPI_78_INT_MASK 14
#define BAS_CSF_LPI_79_INT_MASK 15
#define BAS_CS_LF_0_INT 0
#define BAS_CS_LF_1_INT 1
#define BAS_CS_LF_2_INT 2
#define BAS_CS_LF_3_INT 3
#define BAS_CS_LF_4_INT 4
#define BAS_CS_LF_5_INT 5
#define BAS_CS_LF_6_INT 6
#define BAS_CS_LF_7_INT 7
#define BAS_CS_LF_8_INT 8
#define BAS_CS_LF_9_INT 9
#define BAS_CS_LF_10_INT 10
#define BAS_CS_LF_11_INT 11
#define BAS_CS_LF_12_INT 12
#define BAS_CS_LF_13_INT 13
#define BAS_CS_LF_14_INT 14
#define BAS_CS_LF_15_INT 15
#define BAS_CS_LF_0_INT_MASK 0
#define BAS_CS_LF_1_INT_MASK 1
#define BAS_CS_LF_2_INT_MASK 2
#define BAS_CS_LF_3_INT_MASK 3
#define BAS_CS_LF_4_INT_MASK 4
#define BAS_CS_LF_5_INT_MASK 5
#define BAS_CS_LF_6_INT_MASK 6
#define BAS_CS_LF_7_INT_MASK 7
#define BAS_CS_LF_8_INT_MASK 8
#define BAS_CS_LF_9_INT_MASK 9
#define BAS_CS_LF_10_INT_MASK 10
#define BAS_CS_LF_11_INT_MASK 11
#define BAS_CS_LF_12_INT_MASK 12
#define BAS_CS_LF_13_INT_MASK 13
#define BAS_CS_LF_14_INT_MASK 14
#define BAS_CS_LF_15_INT_MASK 15
#define BAS_CS_LF_16_INT 0
#define BAS_CS_LF_17_INT 1
#define BAS_CS_LF_18_INT 2
#define BAS_CS_LF_19_INT 3
#define BAS_CS_LF_20_INT 4
#define BAS_CS_LF_21_INT 5
#define BAS_CS_LF_22_INT 6
#define BAS_CS_LF_23_INT 7
#define BAS_CS_LF_24_INT 8
#define BAS_CS_LF_25_INT 9
#define BAS_CS_LF_26_INT 10
#define BAS_CS_LF_27_INT 11
#define BAS_CS_LF_28_INT 12
#define BAS_CS_LF_29_INT 13
#define BAS_CS_LF_30_INT 14
#define BAS_CS_LF_31_INT 15
#define BAS_CS_LF_16_INT_MASK 0
#define BAS_CS_LF_17_INT_MASK 1
#define BAS_CS_LF_18_INT_MASK 2
#define BAS_CS_LF_19_INT_MASK 3
#define BAS_CS_LF_20_INT_MASK 4
#define BAS_CS_LF_21_INT_MASK 5
#define BAS_CS_LF_22_INT_MASK 6
#define BAS_CS_LF_23_INT_MASK 7
#define BAS_CS_LF_24_INT_MASK 8
#define BAS_CS_LF_25_INT_MASK 9
#define BAS_CS_LF_26_INT_MASK 10
#define BAS_CS_LF_27_INT_MASK 11
#define BAS_CS_LF_28_INT_MASK 12
#define BAS_CS_LF_29_INT_MASK 13
#define BAS_CS_LF_30_INT_MASK 14
#define BAS_CS_LF_31_INT_MASK 15
#define BAS_CS_LF_32_INT 0
#define BAS_CS_LF_33_INT 1
#define BAS_CS_LF_34_INT 2
#define BAS_CS_LF_35_INT 3
#define BAS_CS_LF_36_INT 4
#define BAS_CS_LF_37_INT 5
#define BAS_CS_LF_38_INT 6
#define BAS_CS_LF_39_INT 7
#define BAS_CS_LF_40_INT 8
#define BAS_CS_LF_41_INT 9
#define BAS_CS_LF_42_INT 10
#define BAS_CS_LF_43_INT 11
#define BAS_CS_LF_44_INT 12
#define BAS_CS_LF_45_INT 13
#define BAS_CS_LF_46_INT 14
#define BAS_CS_LF_47_INT 15
#define BAS_CS_LF_32_INT_MASK 0
#define BAS_CS_LF_33_INT_MASK 1
#define BAS_CS_LF_34_INT_MASK 2
#define BAS_CS_LF_35_INT_MASK 3
#define BAS_CS_LF_36_INT_MASK 4
#define BAS_CS_LF_37_INT_MASK 5
#define BAS_CS_LF_38_INT_MASK 6
#define BAS_CS_LF_39_INT_MASK 7
#define BAS_CS_LF_40_INT_MASK 8
#define BAS_CS_LF_41_INT_MASK 9
#define BAS_CS_LF_42_INT_MASK 10
#define BAS_CS_LF_43_INT_MASK 11
#define BAS_CS_LF_44_INT_MASK 12
#define BAS_CS_LF_45_INT_MASK 13
#define BAS_CS_LF_46_INT_MASK 14
#define BAS_CS_LF_47_INT_MASK 15
#define BAS_CS_LF_48_INT 0
#define BAS_CS_LF_49_INT 1
#define BAS_CS_LF_50_INT 2
#define BAS_CS_LF_51_INT 3
#define BAS_CS_LF_52_INT 4
#define BAS_CS_LF_53_INT 5
#define BAS_CS_LF_54_INT 6
#define BAS_CS_LF_55_INT 7
#define BAS_CS_LF_56_INT 8
#define BAS_CS_LF_57_INT 9
#define BAS_CS_LF_58_INT 10
#define BAS_CS_LF_59_INT 11
#define BAS_CS_LF_60_INT 12
#define BAS_CS_LF_61_INT 13
#define BAS_CS_LF_62_INT 14
#define BAS_CS_LF_63_INT 15
#define BAS_CS_LF_48_INT_MASK 0
#define BAS_CS_LF_49_INT_MASK 1
#define BAS_CS_LF_50_INT_MASK 2
#define BAS_CS_LF_51_INT_MASK 3
#define BAS_CS_LF_52_INT_MASK 4
#define BAS_CS_LF_53_INT_MASK 5
#define BAS_CS_LF_54_INT_MASK 6
#define BAS_CS_LF_55_INT_MASK 7
#define BAS_CS_LF_56_INT_MASK 8
#define BAS_CS_LF_57_INT_MASK 9
#define BAS_CS_LF_58_INT_MASK 10
#define BAS_CS_LF_59_INT_MASK 11
#define BAS_CS_LF_60_INT_MASK 12
#define BAS_CS_LF_61_INT_MASK 13
#define BAS_CS_LF_62_INT_MASK 14
#define BAS_CS_LF_63_INT_MASK 15
#define BAS_CS_LF_64_INT 0
#define BAS_CS_LF_65_INT 1
#define BAS_CS_LF_66_INT 2
#define BAS_CS_LF_67_INT 3
#define BAS_CS_LF_68_INT 4
#define BAS_CS_LF_69_INT 5
#define BAS_CS_LF_70_INT 6
#define BAS_CS_LF_71_INT 7
#define BAS_CS_LF_72_INT 8
#define BAS_CS_LF_73_INT 9
#define BAS_CS_LF_74_INT 10
#define BAS_CS_LF_75_INT 11
#define BAS_CS_LF_76_INT 12
#define BAS_CS_LF_77_INT 13
#define BAS_CS_LF_78_INT 14
#define BAS_CS_LF_79_INT 15
#define BAS_CS_LF_64_INT_MASK 0
#define BAS_CS_LF_65_INT_MASK 1
#define BAS_CS_LF_66_INT_MASK 2
#define BAS_CS_LF_67_INT_MASK 3
#define BAS_CS_LF_68_INT_MASK 4
#define BAS_CS_LF_69_INT_MASK 5
#define BAS_CS_LF_70_INT_MASK 6
#define BAS_CS_LF_71_INT_MASK 7
#define BAS_CS_LF_72_INT_MASK 8
#define BAS_CS_LF_73_INT_MASK 9
#define BAS_CS_LF_74_INT_MASK 10
#define BAS_CS_LF_75_INT_MASK 11
#define BAS_CS_LF_76_INT_MASK 12
#define BAS_CS_LF_77_INT_MASK 13
#define BAS_CS_LF_78_INT_MASK 14
#define BAS_CS_LF_79_INT_MASK 15
#define BAS_CS_RF_0_INT 0
#define BAS_CS_RF_1_INT 1
#define BAS_CS_RF_2_INT 2
#define BAS_CS_RF_3_INT 3
#define BAS_CS_RF_4_INT 4
#define BAS_CS_RF_5_INT 5
#define BAS_CS_RF_6_INT 6
#define BAS_CS_RF_7_INT 7
#define BAS_CS_RF_8_INT 8
#define BAS_CS_RF_9_INT 9
#define BAS_CS_RF_10_INT 10
#define BAS_CS_RF_11_INT 11
#define BAS_CS_RF_12_INT 12
#define BAS_CS_RF_13_INT 13
#define BAS_CS_RF_14_INT 14
#define BAS_CS_RF_15_INT 15
#define BAS_CS_RF_0_INT_MASK 0
#define BAS_CS_RF_1_INT_MASK 1
#define BAS_CS_RF_2_INT_MASK 2
#define BAS_CS_RF_3_INT_MASK 3
#define BAS_CS_RF_4_INT_MASK 4
#define BAS_CS_RF_5_INT_MASK 5
#define BAS_CS_RF_6_INT_MASK 6
#define BAS_CS_RF_7_INT_MASK 7
#define BAS_CS_RF_8_INT_MASK 8
#define BAS_CS_RF_9_INT_MASK 9
#define BAS_CS_RF_10_INT_MASK 10
#define BAS_CS_RF_11_INT_MASK 11
#define BAS_CS_RF_12_INT_MASK 12
#define BAS_CS_RF_13_INT_MASK 13
#define BAS_CS_RF_14_INT_MASK 14
#define BAS_CS_RF_15_INT_MASK 15
#define BAS_CS_RF_16_INT 0
#define BAS_CS_RF_17_INT 1
#define BAS_CS_RF_18_INT 2
#define BAS_CS_RF_19_INT 3
#define BAS_CS_RF_20_INT 4
#define BAS_CS_RF_21_INT 5
#define BAS_CS_RF_22_INT 6
#define BAS_CS_RF_23_INT 7
#define BAS_CS_RF_24_INT 8
#define BAS_CS_RF_25_INT 9
#define BAS_CS_RF_26_INT 10
#define BAS_CS_RF_27_INT 11
#define BAS_CS_RF_28_INT 12
#define BAS_CS_RF_29_INT 13
#define BAS_CS_RF_30_INT 14
#define BAS_CS_RF_31_INT 15
#define BAS_CS_RF_16_INT_MASK 0
#define BAS_CS_RF_17_INT_MASK 1
#define BAS_CS_RF_18_INT_MASK 2
#define BAS_CS_RF_19_INT_MASK 3
#define BAS_CS_RF_20_INT_MASK 4
#define BAS_CS_RF_21_INT_MASK 5
#define BAS_CS_RF_22_INT_MASK 6
#define BAS_CS_RF_23_INT_MASK 7
#define BAS_CS_RF_24_INT_MASK 8
#define BAS_CS_RF_25_INT_MASK 9
#define BAS_CS_RF_26_INT_MASK 10
#define BAS_CS_RF_27_INT_MASK 11
#define BAS_CS_RF_28_INT_MASK 12
#define BAS_CS_RF_29_INT_MASK 13
#define BAS_CS_RF_30_INT_MASK 14
#define BAS_CS_RF_31_INT_MASK 15
#define BAS_CS_RF_32_INT 0
#define BAS_CS_RF_33_INT 1
#define BAS_CS_RF_34_INT 2
#define BAS_CS_RF_35_INT 3
#define BAS_CS_RF_36_INT 4
#define BAS_CS_RF_37_INT 5
#define BAS_CS_RF_38_INT 6
#define BAS_CS_RF_39_INT 7
#define BAS_CS_RF_40_INT 8
#define BAS_CS_RF_41_INT 9
#define BAS_CS_RF_42_INT 10
#define BAS_CS_RF_43_INT 11
#define BAS_CS_RF_44_INT 12
#define BAS_CS_RF_45_INT 13
#define BAS_CS_RF_46_INT 14
#define BAS_CS_RF_47_INT 15
#define BAS_CS_RF_32_INT_MASK 0
#define BAS_CS_RF_33_INT_MASK 1
#define BAS_CS_RF_34_INT_MASK 2
#define BAS_CS_RF_35_INT_MASK 3
#define BAS_CS_RF_36_INT_MASK 4
#define BAS_CS_RF_37_INT_MASK 5
#define BAS_CS_RF_38_INT_MASK 6
#define BAS_CS_RF_39_INT_MASK 7
#define BAS_CS_RF_40_INT_MASK 8
#define BAS_CS_RF_41_INT_MASK 9
#define BAS_CS_RF_42_INT_MASK 10
#define BAS_CS_RF_43_INT_MASK 11
#define BAS_CS_RF_44_INT_MASK 12
#define BAS_CS_RF_45_INT_MASK 13
#define BAS_CS_RF_46_INT_MASK 14
#define BAS_CS_RF_47_INT_MASK 15
#define BAS_CS_RF_48_INT 0
#define BAS_CS_RF_49_INT 1
#define BAS_CS_RF_50_INT 2
#define BAS_CS_RF_51_INT 3
#define BAS_CS_RF_52_INT 4
#define BAS_CS_RF_53_INT 5
#define BAS_CS_RF_54_INT 6
#define BAS_CS_RF_55_INT 7
#define BAS_CS_RF_56_INT 8
#define BAS_CS_RF_57_INT 9
#define BAS_CS_RF_58_INT 10
#define BAS_CS_RF_59_INT 11
#define BAS_CS_RF_60_INT 12
#define BAS_CS_RF_61_INT 13
#define BAS_CS_RF_62_INT 14
#define BAS_CS_RF_63_INT 15
#define BAS_CS_RF_48_INT_MASK 0
#define BAS_CS_RF_49_INT_MASK 1
#define BAS_CS_RF_50_INT_MASK 2
#define BAS_CS_RF_51_INT_MASK 3
#define BAS_CS_RF_52_INT_MASK 4
#define BAS_CS_RF_53_INT_MASK 5
#define BAS_CS_RF_54_INT_MASK 6
#define BAS_CS_RF_55_INT_MASK 7
#define BAS_CS_RF_56_INT_MASK 8
#define BAS_CS_RF_57_INT_MASK 9
#define BAS_CS_RF_58_INT_MASK 10
#define BAS_CS_RF_59_INT_MASK 11
#define BAS_CS_RF_60_INT_MASK 12
#define BAS_CS_RF_61_INT_MASK 13
#define BAS_CS_RF_62_INT_MASK 14
#define BAS_CS_RF_63_INT_MASK 15
#define BAS_CS_RF_64_INT 0
#define BAS_CS_RF_65_INT 1
#define BAS_CS_RF_66_INT 2
#define BAS_CS_RF_67_INT 3
#define BAS_CS_RF_68_INT 4
#define BAS_CS_RF_69_INT 5
#define BAS_CS_RF_70_INT 6
#define BAS_CS_RF_71_INT 7
#define BAS_CS_RF_72_INT 8
#define BAS_CS_RF_73_INT 9
#define BAS_CS_RF_74_INT 10
#define BAS_CS_RF_75_INT 11
#define BAS_CS_RF_76_INT 12
#define BAS_CS_RF_77_INT 13
#define BAS_CS_RF_78_INT 14
#define BAS_CS_RF_79_INT 15
#define BAS_CS_RF_64_INT_MASK 0
#define BAS_CS_RF_65_INT_MASK 1
#define BAS_CS_RF_66_INT_MASK 2
#define BAS_CS_RF_67_INT_MASK 3
#define BAS_CS_RF_68_INT_MASK 4
#define BAS_CS_RF_69_INT_MASK 5
#define BAS_CS_RF_70_INT_MASK 6
#define BAS_CS_RF_71_INT_MASK 7
#define BAS_CS_RF_72_INT_MASK 8
#define BAS_CS_RF_73_INT_MASK 9
#define BAS_CS_RF_74_INT_MASK 10
#define BAS_CS_RF_75_INT_MASK 11
#define BAS_CS_RF_76_INT_MASK 12
#define BAS_CS_RF_77_INT_MASK 13
#define BAS_CS_RF_78_INT_MASK 14
#define BAS_CS_RF_79_INT_MASK 15
#define BAS_CRC_ERR_0_INT 0
#define BAS_CRC_ERR_1_INT 1
#define BAS_CRC_ERR_2_INT 2
#define BAS_CRC_ERR_3_INT 3
#define BAS_CRC_ERR_4_INT 4
#define BAS_CRC_ERR_5_INT 5
#define BAS_CRC_ERR_6_INT 6
#define BAS_CRC_ERR_7_INT 7
#define BAS_CRC_ERR_8_INT 8
#define BAS_CRC_ERR_9_INT 9
#define BAS_CRC_ERR_10_INT 10
#define BAS_CRC_ERR_11_INT 11
#define BAS_CRC_ERR_12_INT 12
#define BAS_CRC_ERR_13_INT 13
#define BAS_CRC_ERR_14_INT 14
#define BAS_CRC_ERR_15_INT 15
#define BAS_CRC_ERR_0_INT_MASK 0
#define BAS_CRC_ERR_1_INT_MASK 1
#define BAS_CRC_ERR_2_INT_MASK 2
#define BAS_CRC_ERR_3_INT_MASK 3
#define BAS_CRC_ERR_4_INT_MASK 4
#define BAS_CRC_ERR_5_INT_MASK 5
#define BAS_CRC_ERR_6_INT_MASK 6
#define BAS_CRC_ERR_7_INT_MASK 7
#define BAS_CRC_ERR_8_INT_MASK 8
#define BAS_CRC_ERR_9_INT_MASK 9
#define BAS_CRC_ERR_10_INT_MASK 10
#define BAS_CRC_ERR_11_INT_MASK 11
#define BAS_CRC_ERR_12_INT_MASK 12
#define BAS_CRC_ERR_13_INT_MASK 13
#define BAS_CRC_ERR_14_INT_MASK 14
#define BAS_CRC_ERR_15_INT_MASK 15
#define BAS_CRC_ERR_16_INT 0
#define BAS_CRC_ERR_17_INT 1
#define BAS_CRC_ERR_18_INT 2
#define BAS_CRC_ERR_19_INT 3
#define BAS_CRC_ERR_20_INT 4
#define BAS_CRC_ERR_21_INT 5
#define BAS_CRC_ERR_22_INT 6
#define BAS_CRC_ERR_23_INT 7
#define BAS_CRC_ERR_24_INT 8
#define BAS_CRC_ERR_25_INT 9
#define BAS_CRC_ERR_26_INT 10
#define BAS_CRC_ERR_27_INT 11
#define BAS_CRC_ERR_28_INT 12
#define BAS_CRC_ERR_29_INT 13
#define BAS_CRC_ERR_30_INT 14
#define BAS_CRC_ERR_31_INT 15
#define BAS_CRC_ERR_16_INT_MASK 0
#define BAS_CRC_ERR_17_INT_MASK 1
#define BAS_CRC_ERR_18_INT_MASK 2
#define BAS_CRC_ERR_19_INT_MASK 3
#define BAS_CRC_ERR_20_INT_MASK 4
#define BAS_CRC_ERR_21_INT_MASK 5
#define BAS_CRC_ERR_22_INT_MASK 6
#define BAS_CRC_ERR_23_INT_MASK 7
#define BAS_CRC_ERR_24_INT_MASK 8
#define BAS_CRC_ERR_25_INT_MASK 9
#define BAS_CRC_ERR_26_INT_MASK 10
#define BAS_CRC_ERR_27_INT_MASK 11
#define BAS_CRC_ERR_28_INT_MASK 12
#define BAS_CRC_ERR_29_INT_MASK 13
#define BAS_CRC_ERR_30_INT_MASK 14
#define BAS_CRC_ERR_31_INT_MASK 15
#define BAS_CRC_ERR_32_INT 0
#define BAS_CRC_ERR_33_INT 1
#define BAS_CRC_ERR_34_INT 2
#define BAS_CRC_ERR_35_INT 3
#define BAS_CRC_ERR_36_INT 4
#define BAS_CRC_ERR_37_INT 5
#define BAS_CRC_ERR_38_INT 6
#define BAS_CRC_ERR_39_INT 7
#define BAS_CRC_ERR_40_INT 8
#define BAS_CRC_ERR_41_INT 9
#define BAS_CRC_ERR_42_INT 10
#define BAS_CRC_ERR_43_INT 11
#define BAS_CRC_ERR_44_INT 12
#define BAS_CRC_ERR_45_INT 13
#define BAS_CRC_ERR_46_INT 14
#define BAS_CRC_ERR_47_INT 15
#define BAS_CRC_ERR_32_INT_MASK 0
#define BAS_CRC_ERR_33_INT_MASK 1
#define BAS_CRC_ERR_34_INT_MASK 2
#define BAS_CRC_ERR_35_INT_MASK 3
#define BAS_CRC_ERR_36_INT_MASK 4
#define BAS_CRC_ERR_37_INT_MASK 5
#define BAS_CRC_ERR_38_INT_MASK 6
#define BAS_CRC_ERR_39_INT_MASK 7
#define BAS_CRC_ERR_40_INT_MASK 8
#define BAS_CRC_ERR_41_INT_MASK 9
#define BAS_CRC_ERR_42_INT_MASK 10
#define BAS_CRC_ERR_43_INT_MASK 11
#define BAS_CRC_ERR_44_INT_MASK 12
#define BAS_CRC_ERR_45_INT_MASK 13
#define BAS_CRC_ERR_46_INT_MASK 14
#define BAS_CRC_ERR_47_INT_MASK 15
#define BAS_CRC_ERR_48_INT 0
#define BAS_CRC_ERR_49_INT 1
#define BAS_CRC_ERR_50_INT 2
#define BAS_CRC_ERR_51_INT 3
#define BAS_CRC_ERR_52_INT 4
#define BAS_CRC_ERR_53_INT 5
#define BAS_CRC_ERR_54_INT 6
#define BAS_CRC_ERR_55_INT 7
#define BAS_CRC_ERR_56_INT 8
#define BAS_CRC_ERR_57_INT 9
#define BAS_CRC_ERR_58_INT 10
#define BAS_CRC_ERR_59_INT 11
#define BAS_CRC_ERR_60_INT 12
#define BAS_CRC_ERR_61_INT 13
#define BAS_CRC_ERR_62_INT 14
#define BAS_CRC_ERR_63_INT 15
#define BAS_CRC_ERR_48_INT_MASK 0
#define BAS_CRC_ERR_49_INT_MASK 1
#define BAS_CRC_ERR_50_INT_MASK 2
#define BAS_CRC_ERR_51_INT_MASK 3
#define BAS_CRC_ERR_52_INT_MASK 4
#define BAS_CRC_ERR_53_INT_MASK 5
#define BAS_CRC_ERR_54_INT_MASK 6
#define BAS_CRC_ERR_55_INT_MASK 7
#define BAS_CRC_ERR_56_INT_MASK 8
#define BAS_CRC_ERR_57_INT_MASK 9
#define BAS_CRC_ERR_58_INT_MASK 10
#define BAS_CRC_ERR_59_INT_MASK 11
#define BAS_CRC_ERR_60_INT_MASK 12
#define BAS_CRC_ERR_61_INT_MASK 13
#define BAS_CRC_ERR_62_INT_MASK 14
#define BAS_CRC_ERR_63_INT_MASK 15
#define BAS_CRC_ERR_64_INT 0
#define BAS_CRC_ERR_65_INT 1
#define BAS_CRC_ERR_66_INT 2
#define BAS_CRC_ERR_67_INT 3
#define BAS_CRC_ERR_68_INT 4
#define BAS_CRC_ERR_69_INT 5
#define BAS_CRC_ERR_70_INT 6
#define BAS_CRC_ERR_71_INT 7
#define BAS_CRC_ERR_72_INT 8
#define BAS_CRC_ERR_73_INT 9
#define BAS_CRC_ERR_74_INT 10
#define BAS_CRC_ERR_75_INT 11
#define BAS_CRC_ERR_76_INT 12
#define BAS_CRC_ERR_77_INT 13
#define BAS_CRC_ERR_78_INT 14
#define BAS_CRC_ERR_79_INT 15
#define BAS_CRC_ERR_64_INT_MASK 0
#define BAS_CRC_ERR_65_INT_MASK 1
#define BAS_CRC_ERR_66_INT_MASK 2
#define BAS_CRC_ERR_67_INT_MASK 3
#define BAS_CRC_ERR_68_INT_MASK 4
#define BAS_CRC_ERR_69_INT_MASK 5
#define BAS_CRC_ERR_70_INT_MASK 6
#define BAS_CRC_ERR_71_INT_MASK 7
#define BAS_CRC_ERR_72_INT_MASK 8
#define BAS_CRC_ERR_73_INT_MASK 9
#define BAS_CRC_ERR_74_INT_MASK 10
#define BAS_CRC_ERR_75_INT_MASK 11
#define BAS_CRC_ERR_76_INT_MASK 12
#define BAS_CRC_ERR_77_INT_MASK 13
#define BAS_CRC_ERR_78_INT_MASK 14
#define BAS_CRC_ERR_79_INT_MASK 15
#define BAS_NO_RECEIVE_ALM_0_INT 0
#define BAS_NO_RECEIVE_ALM_1_INT 1
#define BAS_NO_RECEIVE_ALM_2_INT 2
#define BAS_NO_RECEIVE_ALM_3_INT 3
#define BAS_NO_RECEIVE_ALM_4_INT 4
#define BAS_NO_RECEIVE_ALM_5_INT 5
#define BAS_NO_RECEIVE_ALM_6_INT 6
#define BAS_NO_RECEIVE_ALM_7_INT 7
#define BAS_NO_RECEIVE_ALM_8_INT 8
#define BAS_NO_RECEIVE_ALM_9_INT 9
#define BAS_NO_RECEIVE_ALM_10_INT 10
#define BAS_NO_RECEIVE_ALM_11_INT 11
#define BAS_NO_RECEIVE_ALM_12_INT 12
#define BAS_NO_RECEIVE_ALM_13_INT 13
#define BAS_NO_RECEIVE_ALM_14_INT 14
#define BAS_NO_RECEIVE_ALM_15_INT 15
#define BAS_NO_RECEIVE_ALM_0_INT_MASK 0
#define BAS_NO_RECEIVE_ALM_1_INT_MASK 1
#define BAS_NO_RECEIVE_ALM_2_INT_MASK 2
#define BAS_NO_RECEIVE_ALM_3_INT_MASK 3
#define BAS_NO_RECEIVE_ALM_4_INT_MASK 4
#define BAS_NO_RECEIVE_ALM_5_INT_MASK 5
#define BAS_NO_RECEIVE_ALM_6_INT_MASK 6
#define BAS_NO_RECEIVE_ALM_7_INT_MASK 7
#define BAS_NO_RECEIVE_ALM_8_INT_MASK 8
#define BAS_NO_RECEIVE_ALM_9_INT_MASK 9
#define BAS_NO_RECEIVE_ALM_10_INT_MASK 10
#define BAS_NO_RECEIVE_ALM_11_INT_MASK 11
#define BAS_NO_RECEIVE_ALM_12_INT_MASK 12
#define BAS_NO_RECEIVE_ALM_13_INT_MASK 13
#define BAS_NO_RECEIVE_ALM_14_INT_MASK 14
#define BAS_NO_RECEIVE_ALM_15_INT_MASK 15
#define BAS_NO_RECEIVE_ALM_16_INT 0
#define BAS_NO_RECEIVE_ALM_17_INT 1
#define BAS_NO_RECEIVE_ALM_18_INT 2
#define BAS_NO_RECEIVE_ALM_19_INT 3
#define BAS_NO_RECEIVE_ALM_20_INT 4
#define BAS_NO_RECEIVE_ALM_21_INT 5
#define BAS_NO_RECEIVE_ALM_22_INT 6
#define BAS_NO_RECEIVE_ALM_23_INT 7
#define BAS_NO_RECEIVE_ALM_24_INT 8
#define BAS_NO_RECEIVE_ALM_25_INT 9
#define BAS_NO_RECEIVE_ALM_26_INT 10
#define BAS_NO_RECEIVE_ALM_27_INT 11
#define BAS_NO_RECEIVE_ALM_28_INT 12
#define BAS_NO_RECEIVE_ALM_29_INT 13
#define BAS_NO_RECEIVE_ALM_30_INT 14
#define BAS_NO_RECEIVE_ALM_31_INT 15
#define BAS_NO_RECEIVE_ALM_16_INT_MASK 0
#define BAS_NO_RECEIVE_ALM_17_INT_MASK 1
#define BAS_NO_RECEIVE_ALM_18_INT_MASK 2
#define BAS_NO_RECEIVE_ALM_19_INT_MASK 3
#define BAS_NO_RECEIVE_ALM_20_INT_MASK 4
#define BAS_NO_RECEIVE_ALM_21_INT_MASK 5
#define BAS_NO_RECEIVE_ALM_22_INT_MASK 6
#define BAS_NO_RECEIVE_ALM_23_INT_MASK 7
#define BAS_NO_RECEIVE_ALM_24_INT_MASK 8
#define BAS_NO_RECEIVE_ALM_25_INT_MASK 9
#define BAS_NO_RECEIVE_ALM_26_INT_MASK 10
#define BAS_NO_RECEIVE_ALM_27_INT_MASK 11
#define BAS_NO_RECEIVE_ALM_28_INT_MASK 12
#define BAS_NO_RECEIVE_ALM_29_INT_MASK 13
#define BAS_NO_RECEIVE_ALM_30_INT_MASK 14
#define BAS_NO_RECEIVE_ALM_31_INT_MASK 15
#define BAS_NO_RECEIVE_ALM_32_INT 0
#define BAS_NO_RECEIVE_ALM_33_INT 1
#define BAS_NO_RECEIVE_ALM_34_INT 2
#define BAS_NO_RECEIVE_ALM_35_INT 3
#define BAS_NO_RECEIVE_ALM_36_INT 4
#define BAS_NO_RECEIVE_ALM_37_INT 5
#define BAS_NO_RECEIVE_ALM_38_INT 6
#define BAS_NO_RECEIVE_ALM_39_INT 7
#define BAS_NO_RECEIVE_ALM_40_INT 8
#define BAS_NO_RECEIVE_ALM_41_INT 9
#define BAS_NO_RECEIVE_ALM_42_INT 10
#define BAS_NO_RECEIVE_ALM_43_INT 11
#define BAS_NO_RECEIVE_ALM_44_INT 12
#define BAS_NO_RECEIVE_ALM_45_INT 13
#define BAS_NO_RECEIVE_ALM_46_INT 14
#define BAS_NO_RECEIVE_ALM_47_INT 15
#define BAS_NO_RECEIVE_ALM_32_INT_MASK 0
#define BAS_NO_RECEIVE_ALM_33_INT_MASK 1
#define BAS_NO_RECEIVE_ALM_34_INT_MASK 2
#define BAS_NO_RECEIVE_ALM_35_INT_MASK 3
#define BAS_NO_RECEIVE_ALM_36_INT_MASK 4
#define BAS_NO_RECEIVE_ALM_37_INT_MASK 5
#define BAS_NO_RECEIVE_ALM_38_INT_MASK 6
#define BAS_NO_RECEIVE_ALM_39_INT_MASK 7
#define BAS_NO_RECEIVE_ALM_40_INT_MASK 8
#define BAS_NO_RECEIVE_ALM_41_INT_MASK 9
#define BAS_NO_RECEIVE_ALM_42_INT_MASK 10
#define BAS_NO_RECEIVE_ALM_43_INT_MASK 11
#define BAS_NO_RECEIVE_ALM_44_INT_MASK 12
#define BAS_NO_RECEIVE_ALM_45_INT_MASK 13
#define BAS_NO_RECEIVE_ALM_46_INT_MASK 14
#define BAS_NO_RECEIVE_ALM_47_INT_MASK 15
#define BAS_NO_RECEIVE_ALM_48_INT 0
#define BAS_NO_RECEIVE_ALM_49_INT 1
#define BAS_NO_RECEIVE_ALM_50_INT 2
#define BAS_NO_RECEIVE_ALM_51_INT 3
#define BAS_NO_RECEIVE_ALM_52_INT 4
#define BAS_NO_RECEIVE_ALM_53_INT 5
#define BAS_NO_RECEIVE_ALM_54_INT 6
#define BAS_NO_RECEIVE_ALM_55_INT 7
#define BAS_NO_RECEIVE_ALM_56_INT 8
#define BAS_NO_RECEIVE_ALM_57_INT 9
#define BAS_NO_RECEIVE_ALM_58_INT 10
#define BAS_NO_RECEIVE_ALM_59_INT 11
#define BAS_NO_RECEIVE_ALM_60_INT 12
#define BAS_NO_RECEIVE_ALM_61_INT 13
#define BAS_NO_RECEIVE_ALM_62_INT 14
#define BAS_NO_RECEIVE_ALM_63_INT 15
#define BAS_NO_RECEIVE_ALM_48_INT_MASK 0
#define BAS_NO_RECEIVE_ALM_49_INT_MASK 1
#define BAS_NO_RECEIVE_ALM_50_INT_MASK 2
#define BAS_NO_RECEIVE_ALM_51_INT_MASK 3
#define BAS_NO_RECEIVE_ALM_52_INT_MASK 4
#define BAS_NO_RECEIVE_ALM_53_INT_MASK 5
#define BAS_NO_RECEIVE_ALM_54_INT_MASK 6
#define BAS_NO_RECEIVE_ALM_55_INT_MASK 7
#define BAS_NO_RECEIVE_ALM_56_INT_MASK 8
#define BAS_NO_RECEIVE_ALM_57_INT_MASK 9
#define BAS_NO_RECEIVE_ALM_58_INT_MASK 10
#define BAS_NO_RECEIVE_ALM_59_INT_MASK 11
#define BAS_NO_RECEIVE_ALM_60_INT_MASK 12
#define BAS_NO_RECEIVE_ALM_61_INT_MASK 13
#define BAS_NO_RECEIVE_ALM_62_INT_MASK 14
#define BAS_NO_RECEIVE_ALM_63_INT_MASK 15
#define BAS_NO_RECEIVE_ALM_64_INT 0
#define BAS_NO_RECEIVE_ALM_65_INT 1
#define BAS_NO_RECEIVE_ALM_66_INT 2
#define BAS_NO_RECEIVE_ALM_67_INT 3
#define BAS_NO_RECEIVE_ALM_68_INT 4
#define BAS_NO_RECEIVE_ALM_69_INT 5
#define BAS_NO_RECEIVE_ALM_70_INT 6
#define BAS_NO_RECEIVE_ALM_71_INT 7
#define BAS_NO_RECEIVE_ALM_72_INT 8
#define BAS_NO_RECEIVE_ALM_73_INT 9
#define BAS_NO_RECEIVE_ALM_74_INT 10
#define BAS_NO_RECEIVE_ALM_75_INT 11
#define BAS_NO_RECEIVE_ALM_76_INT 12
#define BAS_NO_RECEIVE_ALM_77_INT 13
#define BAS_NO_RECEIVE_ALM_78_INT 14
#define BAS_NO_RECEIVE_ALM_79_INT 15
#define BAS_NO_RECEIVE_ALM_64_INT_MASK 0
#define BAS_NO_RECEIVE_ALM_65_INT_MASK 1
#define BAS_NO_RECEIVE_ALM_66_INT_MASK 2
#define BAS_NO_RECEIVE_ALM_67_INT_MASK 3
#define BAS_NO_RECEIVE_ALM_68_INT_MASK 4
#define BAS_NO_RECEIVE_ALM_69_INT_MASK 5
#define BAS_NO_RECEIVE_ALM_70_INT_MASK 6
#define BAS_NO_RECEIVE_ALM_71_INT_MASK 7
#define BAS_NO_RECEIVE_ALM_72_INT_MASK 8
#define BAS_NO_RECEIVE_ALM_73_INT_MASK 9
#define BAS_NO_RECEIVE_ALM_74_INT_MASK 10
#define BAS_NO_RECEIVE_ALM_75_INT_MASK 11
#define BAS_NO_RECEIVE_ALM_76_INT_MASK 12
#define BAS_NO_RECEIVE_ALM_77_INT_MASK 13
#define BAS_NO_RECEIVE_ALM_78_INT_MASK 14
#define BAS_NO_RECEIVE_ALM_79_INT_MASK 15
#define RDI_LF_0_INT 0
#define RDI_LF_1_INT 1
#define RDI_LF_2_INT 2
#define RDI_LF_3_INT 3
#define RDI_LF_4_INT 4
#define RDI_LF_5_INT 5
#define RDI_LF_6_INT 6
#define RDI_LF_7_INT 7
#define RDI_LF_8_INT 8
#define RDI_LF_9_INT 9
#define RDI_LF_10_INT 10
#define RDI_LF_11_INT 11
#define RDI_LF_12_INT 12
#define RDI_LF_13_INT 13
#define RDI_LF_14_INT 14
#define RDI_LF_15_INT 15
#define RDI_LF_0_INT_MASK 0
#define RDI_LF_1_INT_MASK 1
#define RDI_LF_2_INT_MASK 2
#define RDI_LF_3_INT_MASK 3
#define RDI_LF_4_INT_MASK 4
#define RDI_LF_5_INT_MASK 5
#define RDI_LF_6_INT_MASK 6
#define RDI_LF_7_INT_MASK 7
#define RDI_LF_8_INT_MASK 8
#define RDI_LF_9_INT_MASK 9
#define RDI_LF_10_INT_MASK 10
#define RDI_LF_11_INT_MASK 11
#define RDI_LF_12_INT_MASK 12
#define RDI_LF_13_INT_MASK 13
#define RDI_LF_14_INT_MASK 14
#define RDI_LF_15_INT_MASK 15
#define RDI_LF_16_INT 0
#define RDI_LF_17_INT 1
#define RDI_LF_18_INT 2
#define RDI_LF_19_INT 3
#define RDI_LF_20_INT 4
#define RDI_LF_21_INT 5
#define RDI_LF_22_INT 6
#define RDI_LF_23_INT 7
#define RDI_LF_24_INT 8
#define RDI_LF_25_INT 9
#define RDI_LF_26_INT 10
#define RDI_LF_27_INT 11
#define RDI_LF_28_INT 12
#define RDI_LF_29_INT 13
#define RDI_LF_30_INT 14
#define RDI_LF_31_INT 15
#define RDI_LF_16_INT_MASK 0
#define RDI_LF_17_INT_MASK 1
#define RDI_LF_18_INT_MASK 2
#define RDI_LF_19_INT_MASK 3
#define RDI_LF_20_INT_MASK 4
#define RDI_LF_21_INT_MASK 5
#define RDI_LF_22_INT_MASK 6
#define RDI_LF_23_INT_MASK 7
#define RDI_LF_24_INT_MASK 8
#define RDI_LF_25_INT_MASK 9
#define RDI_LF_26_INT_MASK 10
#define RDI_LF_27_INT_MASK 11
#define RDI_LF_28_INT_MASK 12
#define RDI_LF_29_INT_MASK 13
#define RDI_LF_30_INT_MASK 14
#define RDI_LF_31_INT_MASK 15
#define RDI_LF_32_INT 0
#define RDI_LF_33_INT 1
#define RDI_LF_34_INT 2
#define RDI_LF_35_INT 3
#define RDI_LF_36_INT 4
#define RDI_LF_37_INT 5
#define RDI_LF_38_INT 6
#define RDI_LF_39_INT 7
#define RDI_LF_40_INT 8
#define RDI_LF_41_INT 9
#define RDI_LF_42_INT 10
#define RDI_LF_43_INT 11
#define RDI_LF_44_INT 12
#define RDI_LF_45_INT 13
#define RDI_LF_46_INT 14
#define RDI_LF_47_INT 15
#define RDI_LF_32_INT_MASK 0
#define RDI_LF_33_INT_MASK 1
#define RDI_LF_34_INT_MASK 2
#define RDI_LF_35_INT_MASK 3
#define RDI_LF_36_INT_MASK 4
#define RDI_LF_37_INT_MASK 5
#define RDI_LF_38_INT_MASK 6
#define RDI_LF_39_INT_MASK 7
#define RDI_LF_40_INT_MASK 8
#define RDI_LF_41_INT_MASK 9
#define RDI_LF_42_INT_MASK 10
#define RDI_LF_43_INT_MASK 11
#define RDI_LF_44_INT_MASK 12
#define RDI_LF_45_INT_MASK 13
#define RDI_LF_46_INT_MASK 14
#define RDI_LF_47_INT_MASK 15
#define RDI_LF_48_INT 0
#define RDI_LF_49_INT 1
#define RDI_LF_50_INT 2
#define RDI_LF_51_INT 3
#define RDI_LF_52_INT 4
#define RDI_LF_53_INT 5
#define RDI_LF_54_INT 6
#define RDI_LF_55_INT 7
#define RDI_LF_56_INT 8
#define RDI_LF_57_INT 9
#define RDI_LF_58_INT 10
#define RDI_LF_59_INT 11
#define RDI_LF_60_INT 12
#define RDI_LF_61_INT 13
#define RDI_LF_62_INT 14
#define RDI_LF_63_INT 15
#define RDI_LF_48_INT_MASK 0
#define RDI_LF_49_INT_MASK 1
#define RDI_LF_50_INT_MASK 2
#define RDI_LF_51_INT_MASK 3
#define RDI_LF_52_INT_MASK 4
#define RDI_LF_53_INT_MASK 5
#define RDI_LF_54_INT_MASK 6
#define RDI_LF_55_INT_MASK 7
#define RDI_LF_56_INT_MASK 8
#define RDI_LF_57_INT_MASK 9
#define RDI_LF_58_INT_MASK 10
#define RDI_LF_59_INT_MASK 11
#define RDI_LF_60_INT_MASK 12
#define RDI_LF_61_INT_MASK 13
#define RDI_LF_62_INT_MASK 14
#define RDI_LF_63_INT_MASK 15
#define RDI_LF_64_INT 0
#define RDI_LF_65_INT 1
#define RDI_LF_66_INT 2
#define RDI_LF_67_INT 3
#define RDI_LF_68_INT 4
#define RDI_LF_69_INT 5
#define RDI_LF_70_INT 6
#define RDI_LF_71_INT 7
#define RDI_LF_72_INT 8
#define RDI_LF_73_INT 9
#define RDI_LF_74_INT 10
#define RDI_LF_75_INT 11
#define RDI_LF_76_INT 12
#define RDI_LF_77_INT 13
#define RDI_LF_78_INT 14
#define RDI_LF_79_INT 15
#define RDI_LF_64_INT_MASK 0
#define RDI_LF_65_INT_MASK 1
#define RDI_LF_66_INT_MASK 2
#define RDI_LF_67_INT_MASK 3
#define RDI_LF_68_INT_MASK 4
#define RDI_LF_69_INT_MASK 5
#define RDI_LF_70_INT_MASK 6
#define RDI_LF_71_INT_MASK 7
#define RDI_LF_72_INT_MASK 8
#define RDI_LF_73_INT_MASK 9
#define RDI_LF_74_INT_MASK 10
#define RDI_LF_75_INT_MASK 11
#define RDI_LF_76_INT_MASK 12
#define RDI_LF_77_INT_MASK 13
#define RDI_LF_78_INT_MASK 14
#define RDI_LF_79_INT_MASK 15
#define RX_SDBEI_0_INT 0
#define RX_SDBEI_1_INT 1
#define RX_SDBEI_2_INT 2
#define RX_SDBEI_3_INT 3
#define RX_SDBEI_4_INT 4
#define RX_SDBEI_5_INT 5
#define RX_SDBEI_6_INT 6
#define RX_SDBEI_7_INT 7
#define RX_SDBEI_8_INT 8
#define RX_SDBEI_9_INT 9
#define RX_SDBEI_10_INT 10
#define RX_SDBEI_11_INT 11
#define RX_SDBEI_12_INT 12
#define RX_SDBEI_13_INT 13
#define RX_SDBEI_14_INT 14
#define RX_SDBEI_15_INT 15
#define RX_SDBEI_0_INT_MASK 0
#define RX_SDBEI_1_INT_MASK 1
#define RX_SDBEI_2_INT_MASK 2
#define RX_SDBEI_3_INT_MASK 3
#define RX_SDBEI_4_INT_MASK 4
#define RX_SDBEI_5_INT_MASK 5
#define RX_SDBEI_6_INT_MASK 6
#define RX_SDBEI_7_INT_MASK 7
#define RX_SDBEI_8_INT_MASK 8
#define RX_SDBEI_9_INT_MASK 9
#define RX_SDBEI_10_INT_MASK 10
#define RX_SDBEI_11_INT_MASK 11
#define RX_SDBEI_12_INT_MASK 12
#define RX_SDBEI_13_INT_MASK 13
#define RX_SDBEI_14_INT_MASK 14
#define RX_SDBEI_15_INT_MASK 15
#define RX_SDBEI_16_INT 0
#define RX_SDBEI_17_INT 1
#define RX_SDBEI_18_INT 2
#define RX_SDBEI_19_INT 3
#define RX_SDBEI_20_INT 4
#define RX_SDBEI_21_INT 5
#define RX_SDBEI_22_INT 6
#define RX_SDBEI_23_INT 7
#define RX_SDBEI_24_INT 8
#define RX_SDBEI_25_INT 9
#define RX_SDBEI_26_INT 10
#define RX_SDBEI_27_INT 11
#define RX_SDBEI_28_INT 12
#define RX_SDBEI_29_INT 13
#define RX_SDBEI_30_INT 14
#define RX_SDBEI_31_INT 15
#define RX_SDBEI_16_INT_MASK 0
#define RX_SDBEI_17_INT_MASK 1
#define RX_SDBEI_18_INT_MASK 2
#define RX_SDBEI_19_INT_MASK 3
#define RX_SDBEI_20_INT_MASK 4
#define RX_SDBEI_21_INT_MASK 5
#define RX_SDBEI_22_INT_MASK 6
#define RX_SDBEI_23_INT_MASK 7
#define RX_SDBEI_24_INT_MASK 8
#define RX_SDBEI_25_INT_MASK 9
#define RX_SDBEI_26_INT_MASK 10
#define RX_SDBEI_27_INT_MASK 11
#define RX_SDBEI_28_INT_MASK 12
#define RX_SDBEI_29_INT_MASK 13
#define RX_SDBEI_30_INT_MASK 14
#define RX_SDBEI_31_INT_MASK 15
#define RX_SDBEI_32_INT 0
#define RX_SDBEI_33_INT 1
#define RX_SDBEI_34_INT 2
#define RX_SDBEI_35_INT 3
#define RX_SDBEI_36_INT 4
#define RX_SDBEI_37_INT 5
#define RX_SDBEI_38_INT 6
#define RX_SDBEI_39_INT 7
#define RX_SDBEI_40_INT 8
#define RX_SDBEI_41_INT 9
#define RX_SDBEI_42_INT 10
#define RX_SDBEI_43_INT 11
#define RX_SDBEI_44_INT 12
#define RX_SDBEI_45_INT 13
#define RX_SDBEI_46_INT 14
#define RX_SDBEI_47_INT 15
#define RX_SDBEI_32_INT_MASK 0
#define RX_SDBEI_33_INT_MASK 1
#define RX_SDBEI_34_INT_MASK 2
#define RX_SDBEI_35_INT_MASK 3
#define RX_SDBEI_36_INT_MASK 4
#define RX_SDBEI_37_INT_MASK 5
#define RX_SDBEI_38_INT_MASK 6
#define RX_SDBEI_39_INT_MASK 7
#define RX_SDBEI_40_INT_MASK 8
#define RX_SDBEI_41_INT_MASK 9
#define RX_SDBEI_42_INT_MASK 10
#define RX_SDBEI_43_INT_MASK 11
#define RX_SDBEI_44_INT_MASK 12
#define RX_SDBEI_45_INT_MASK 13
#define RX_SDBEI_46_INT_MASK 14
#define RX_SDBEI_47_INT_MASK 15
#define RX_SDBEI_48_INT 0
#define RX_SDBEI_49_INT 1
#define RX_SDBEI_50_INT 2
#define RX_SDBEI_51_INT 3
#define RX_SDBEI_52_INT 4
#define RX_SDBEI_53_INT 5
#define RX_SDBEI_54_INT 6
#define RX_SDBEI_55_INT 7
#define RX_SDBEI_56_INT 8
#define RX_SDBEI_57_INT 9
#define RX_SDBEI_58_INT 10
#define RX_SDBEI_59_INT 11
#define RX_SDBEI_60_INT 12
#define RX_SDBEI_61_INT 13
#define RX_SDBEI_62_INT 14
#define RX_SDBEI_63_INT 15
#define RX_SDBEI_48_INT_MASK 0
#define RX_SDBEI_49_INT_MASK 1
#define RX_SDBEI_50_INT_MASK 2
#define RX_SDBEI_51_INT_MASK 3
#define RX_SDBEI_52_INT_MASK 4
#define RX_SDBEI_53_INT_MASK 5
#define RX_SDBEI_54_INT_MASK 6
#define RX_SDBEI_55_INT_MASK 7
#define RX_SDBEI_56_INT_MASK 8
#define RX_SDBEI_57_INT_MASK 9
#define RX_SDBEI_58_INT_MASK 10
#define RX_SDBEI_59_INT_MASK 11
#define RX_SDBEI_60_INT_MASK 12
#define RX_SDBEI_61_INT_MASK 13
#define RX_SDBEI_62_INT_MASK 14
#define RX_SDBEI_63_INT_MASK 15
#define RX_SDBEI_64_INT 0
#define RX_SDBEI_65_INT 1
#define RX_SDBEI_66_INT 2
#define RX_SDBEI_67_INT 3
#define RX_SDBEI_68_INT 4
#define RX_SDBEI_69_INT 5
#define RX_SDBEI_70_INT 6
#define RX_SDBEI_71_INT 7
#define RX_SDBEI_72_INT 8
#define RX_SDBEI_73_INT 9
#define RX_SDBEI_74_INT 10
#define RX_SDBEI_75_INT 11
#define RX_SDBEI_76_INT 12
#define RX_SDBEI_77_INT 13
#define RX_SDBEI_78_INT 14
#define RX_SDBEI_79_INT 15
#define RX_SDBEI_64_INT_MASK 0
#define RX_SDBEI_65_INT_MASK 1
#define RX_SDBEI_66_INT_MASK 2
#define RX_SDBEI_67_INT_MASK 3
#define RX_SDBEI_68_INT_MASK 4
#define RX_SDBEI_69_INT_MASK 5
#define RX_SDBEI_70_INT_MASK 6
#define RX_SDBEI_71_INT_MASK 7
#define RX_SDBEI_72_INT_MASK 8
#define RX_SDBEI_73_INT_MASK 9
#define RX_SDBEI_74_INT_MASK 10
#define RX_SDBEI_75_INT_MASK 11
#define RX_SDBEI_76_INT_MASK 12
#define RX_SDBEI_77_INT_MASK 13
#define RX_SDBEI_78_INT_MASK 14
#define RX_SDBEI_79_INT_MASK 15
#define RF_ALM_0_INT 0
#define RF_ALM_1_INT 1
#define RF_ALM_2_INT 2
#define RF_ALM_3_INT 3
#define RF_ALM_4_INT 4
#define RF_ALM_5_INT 5
#define RF_ALM_6_INT 6
#define RF_ALM_7_INT 7
#define RF_ALM_8_INT 8
#define RF_ALM_9_INT 9
#define RF_ALM_10_INT 10
#define RF_ALM_11_INT 11
#define RF_ALM_12_INT 12
#define RF_ALM_13_INT 13
#define RF_ALM_14_INT 14
#define RF_ALM_15_INT 15
#define RF_ALM_0_INT_MASK 0
#define RF_ALM_1_INT_MASK 1
#define RF_ALM_2_INT_MASK 2
#define RF_ALM_3_INT_MASK 3
#define RF_ALM_4_INT_MASK 4
#define RF_ALM_5_INT_MASK 5
#define RF_ALM_6_INT_MASK 6
#define RF_ALM_7_INT_MASK 7
#define RF_ALM_8_INT_MASK 8
#define RF_ALM_9_INT_MASK 9
#define RF_ALM_10_INT_MASK 10
#define RF_ALM_11_INT_MASK 11
#define RF_ALM_12_INT_MASK 12
#define RF_ALM_13_INT_MASK 13
#define RF_ALM_14_INT_MASK 14
#define RF_ALM_15_INT_MASK 15
#define RF_ALM_16_INT 0
#define RF_ALM_17_INT 1
#define RF_ALM_18_INT 2
#define RF_ALM_19_INT 3
#define RF_ALM_20_INT 4
#define RF_ALM_21_INT 5
#define RF_ALM_22_INT 6
#define RF_ALM_23_INT 7
#define RF_ALM_24_INT 8
#define RF_ALM_25_INT 9
#define RF_ALM_26_INT 10
#define RF_ALM_27_INT 11
#define RF_ALM_28_INT 12
#define RF_ALM_29_INT 13
#define RF_ALM_30_INT 14
#define RF_ALM_31_INT 15
#define RF_ALM_16_INT_MASK 0
#define RF_ALM_17_INT_MASK 1
#define RF_ALM_18_INT_MASK 2
#define RF_ALM_19_INT_MASK 3
#define RF_ALM_20_INT_MASK 4
#define RF_ALM_21_INT_MASK 5
#define RF_ALM_22_INT_MASK 6
#define RF_ALM_23_INT_MASK 7
#define RF_ALM_24_INT_MASK 8
#define RF_ALM_25_INT_MASK 9
#define RF_ALM_26_INT_MASK 10
#define RF_ALM_27_INT_MASK 11
#define RF_ALM_28_INT_MASK 12
#define RF_ALM_29_INT_MASK 13
#define RF_ALM_30_INT_MASK 14
#define RF_ALM_31_INT_MASK 15
#define RF_ALM_32_INT 0
#define RF_ALM_33_INT 1
#define RF_ALM_34_INT 2
#define RF_ALM_35_INT 3
#define RF_ALM_36_INT 4
#define RF_ALM_37_INT 5
#define RF_ALM_38_INT 6
#define RF_ALM_39_INT 7
#define RF_ALM_40_INT 8
#define RF_ALM_41_INT 9
#define RF_ALM_42_INT 10
#define RF_ALM_43_INT 11
#define RF_ALM_44_INT 12
#define RF_ALM_45_INT 13
#define RF_ALM_46_INT 14
#define RF_ALM_47_INT 15
#define RF_ALM_32_INT_MASK 0
#define RF_ALM_33_INT_MASK 1
#define RF_ALM_34_INT_MASK 2
#define RF_ALM_35_INT_MASK 3
#define RF_ALM_36_INT_MASK 4
#define RF_ALM_37_INT_MASK 5
#define RF_ALM_38_INT_MASK 6
#define RF_ALM_39_INT_MASK 7
#define RF_ALM_40_INT_MASK 8
#define RF_ALM_41_INT_MASK 9
#define RF_ALM_42_INT_MASK 10
#define RF_ALM_43_INT_MASK 11
#define RF_ALM_44_INT_MASK 12
#define RF_ALM_45_INT_MASK 13
#define RF_ALM_46_INT_MASK 14
#define RF_ALM_47_INT_MASK 15
#define RF_ALM_48_INT 0
#define RF_ALM_49_INT 1
#define RF_ALM_50_INT 2
#define RF_ALM_51_INT 3
#define RF_ALM_52_INT 4
#define RF_ALM_53_INT 5
#define RF_ALM_54_INT 6
#define RF_ALM_55_INT 7
#define RF_ALM_56_INT 8
#define RF_ALM_57_INT 9
#define RF_ALM_58_INT 10
#define RF_ALM_59_INT 11
#define RF_ALM_60_INT 12
#define RF_ALM_61_INT 13
#define RF_ALM_62_INT 14
#define RF_ALM_63_INT 15
#define RF_ALM_48_INT_MASK 0
#define RF_ALM_49_INT_MASK 1
#define RF_ALM_50_INT_MASK 2
#define RF_ALM_51_INT_MASK 3
#define RF_ALM_52_INT_MASK 4
#define RF_ALM_53_INT_MASK 5
#define RF_ALM_54_INT_MASK 6
#define RF_ALM_55_INT_MASK 7
#define RF_ALM_56_INT_MASK 8
#define RF_ALM_57_INT_MASK 9
#define RF_ALM_58_INT_MASK 10
#define RF_ALM_59_INT_MASK 11
#define RF_ALM_60_INT_MASK 12
#define RF_ALM_61_INT_MASK 13
#define RF_ALM_62_INT_MASK 14
#define RF_ALM_63_INT_MASK 15
#define RF_ALM_64_INT 0
#define RF_ALM_65_INT 1
#define RF_ALM_66_INT 2
#define RF_ALM_67_INT 3
#define RF_ALM_68_INT 4
#define RF_ALM_69_INT 5
#define RF_ALM_70_INT 6
#define RF_ALM_71_INT 7
#define RF_ALM_72_INT 8
#define RF_ALM_73_INT 9
#define RF_ALM_74_INT 10
#define RF_ALM_75_INT 11
#define RF_ALM_76_INT 12
#define RF_ALM_77_INT 13
#define RF_ALM_78_INT 14
#define RF_ALM_79_INT 15
#define RF_ALM_64_INT_MASK 0
#define RF_ALM_65_INT_MASK 1
#define RF_ALM_66_INT_MASK 2
#define RF_ALM_67_INT_MASK 3
#define RF_ALM_68_INT_MASK 4
#define RF_ALM_69_INT_MASK 5
#define RF_ALM_70_INT_MASK 6
#define RF_ALM_71_INT_MASK 7
#define RF_ALM_72_INT_MASK 8
#define RF_ALM_73_INT_MASK 9
#define RF_ALM_74_INT_MASK 10
#define RF_ALM_75_INT_MASK 11
#define RF_ALM_76_INT_MASK 12
#define RF_ALM_77_INT_MASK 13
#define RF_ALM_78_INT_MASK 14
#define RF_ALM_79_INT_MASK 15
#define LPI_ALM_0_INT 0
#define LPI_ALM_1_INT 1
#define LPI_ALM_2_INT 2
#define LPI_ALM_3_INT 3
#define LPI_ALM_4_INT 4
#define LPI_ALM_5_INT 5
#define LPI_ALM_6_INT 6
#define LPI_ALM_7_INT 7
#define LPI_ALM_8_INT 8
#define LPI_ALM_9_INT 9
#define LPI_ALM_10_INT 10
#define LPI_ALM_11_INT 11
#define LPI_ALM_12_INT 12
#define LPI_ALM_13_INT 13
#define LPI_ALM_14_INT 14
#define LPI_ALM_15_INT 15
#define LPI_ALM_0_INT_MASK 0
#define LPI_ALM_1_INT_MASK 1
#define LPI_ALM_2_INT_MASK 2
#define LPI_ALM_3_INT_MASK 3
#define LPI_ALM_4_INT_MASK 4
#define LPI_ALM_5_INT_MASK 5
#define LPI_ALM_6_INT_MASK 6
#define LPI_ALM_7_INT_MASK 7
#define LPI_ALM_8_INT_MASK 8
#define LPI_ALM_9_INT_MASK 9
#define LPI_ALM_10_INT_MASK 10
#define LPI_ALM_11_INT_MASK 11
#define LPI_ALM_12_INT_MASK 12
#define LPI_ALM_13_INT_MASK 13
#define LPI_ALM_14_INT_MASK 14
#define LPI_ALM_15_INT_MASK 15
#define LPI_ALM_16_INT 0
#define LPI_ALM_17_INT 1
#define LPI_ALM_18_INT 2
#define LPI_ALM_19_INT 3
#define LPI_ALM_20_INT 4
#define LPI_ALM_21_INT 5
#define LPI_ALM_22_INT 6
#define LPI_ALM_23_INT 7
#define LPI_ALM_24_INT 8
#define LPI_ALM_25_INT 9
#define LPI_ALM_26_INT 10
#define LPI_ALM_27_INT 11
#define LPI_ALM_28_INT 12
#define LPI_ALM_29_INT 13
#define LPI_ALM_30_INT 14
#define LPI_ALM_31_INT 15
#define LPI_ALM_16_INT_MASK 0
#define LPI_ALM_17_INT_MASK 1
#define LPI_ALM_18_INT_MASK 2
#define LPI_ALM_19_INT_MASK 3
#define LPI_ALM_20_INT_MASK 4
#define LPI_ALM_21_INT_MASK 5
#define LPI_ALM_22_INT_MASK 6
#define LPI_ALM_23_INT_MASK 7
#define LPI_ALM_24_INT_MASK 8
#define LPI_ALM_25_INT_MASK 9
#define LPI_ALM_26_INT_MASK 10
#define LPI_ALM_27_INT_MASK 11
#define LPI_ALM_28_INT_MASK 12
#define LPI_ALM_29_INT_MASK 13
#define LPI_ALM_30_INT_MASK 14
#define LPI_ALM_31_INT_MASK 15
#define LPI_ALM_32_INT 0
#define LPI_ALM_33_INT 1
#define LPI_ALM_34_INT 2
#define LPI_ALM_35_INT 3
#define LPI_ALM_36_INT 4
#define LPI_ALM_37_INT 5
#define LPI_ALM_38_INT 6
#define LPI_ALM_39_INT 7
#define LPI_ALM_40_INT 8
#define LPI_ALM_41_INT 9
#define LPI_ALM_42_INT 10
#define LPI_ALM_43_INT 11
#define LPI_ALM_44_INT 12
#define LPI_ALM_45_INT 13
#define LPI_ALM_46_INT 14
#define LPI_ALM_47_INT 15
#define LPI_ALM_32_INT_MASK 0
#define LPI_ALM_33_INT_MASK 1
#define LPI_ALM_34_INT_MASK 2
#define LPI_ALM_35_INT_MASK 3
#define LPI_ALM_36_INT_MASK 4
#define LPI_ALM_37_INT_MASK 5
#define LPI_ALM_38_INT_MASK 6
#define LPI_ALM_39_INT_MASK 7
#define LPI_ALM_40_INT_MASK 8
#define LPI_ALM_41_INT_MASK 9
#define LPI_ALM_42_INT_MASK 10
#define LPI_ALM_43_INT_MASK 11
#define LPI_ALM_44_INT_MASK 12
#define LPI_ALM_45_INT_MASK 13
#define LPI_ALM_46_INT_MASK 14
#define LPI_ALM_47_INT_MASK 15
#define LPI_ALM_48_INT 0
#define LPI_ALM_49_INT 1
#define LPI_ALM_50_INT 2
#define LPI_ALM_51_INT 3
#define LPI_ALM_52_INT 4
#define LPI_ALM_53_INT 5
#define LPI_ALM_54_INT 6
#define LPI_ALM_55_INT 7
#define LPI_ALM_56_INT 8
#define LPI_ALM_57_INT 9
#define LPI_ALM_58_INT 10
#define LPI_ALM_59_INT 11
#define LPI_ALM_60_INT 12
#define LPI_ALM_61_INT 13
#define LPI_ALM_62_INT 14
#define LPI_ALM_63_INT 15
#define LPI_ALM_48_INT_MASK 0
#define LPI_ALM_49_INT_MASK 1
#define LPI_ALM_50_INT_MASK 2
#define LPI_ALM_51_INT_MASK 3
#define LPI_ALM_52_INT_MASK 4
#define LPI_ALM_53_INT_MASK 5
#define LPI_ALM_54_INT_MASK 6
#define LPI_ALM_55_INT_MASK 7
#define LPI_ALM_56_INT_MASK 8
#define LPI_ALM_57_INT_MASK 9
#define LPI_ALM_58_INT_MASK 10
#define LPI_ALM_59_INT_MASK 11
#define LPI_ALM_60_INT_MASK 12
#define LPI_ALM_61_INT_MASK 13
#define LPI_ALM_62_INT_MASK 14
#define LPI_ALM_63_INT_MASK 15
#define LPI_ALM_64_INT 0
#define LPI_ALM_65_INT 1
#define LPI_ALM_66_INT 2
#define LPI_ALM_67_INT 3
#define LPI_ALM_68_INT 4
#define LPI_ALM_69_INT 5
#define LPI_ALM_70_INT 6
#define LPI_ALM_71_INT 7
#define LPI_ALM_72_INT 8
#define LPI_ALM_73_INT 9
#define LPI_ALM_74_INT 10
#define LPI_ALM_75_INT 11
#define LPI_ALM_76_INT 12
#define LPI_ALM_77_INT 13
#define LPI_ALM_78_INT 14
#define LPI_ALM_79_INT 15
#define LPI_ALM_64_INT_MASK 0
#define LPI_ALM_65_INT_MASK 1
#define LPI_ALM_66_INT_MASK 2
#define LPI_ALM_67_INT_MASK 3
#define LPI_ALM_68_INT_MASK 4
#define LPI_ALM_69_INT_MASK 5
#define LPI_ALM_70_INT_MASK 6
#define LPI_ALM_71_INT_MASK 7
#define LPI_ALM_72_INT_MASK 8
#define LPI_ALM_73_INT_MASK 9
#define LPI_ALM_74_INT_MASK 10
#define LPI_ALM_75_INT_MASK 11
#define LPI_ALM_76_INT_MASK 12
#define LPI_ALM_77_INT_MASK 13
#define LPI_ALM_78_INT_MASK 14
#define LPI_ALM_79_INT_MASK 15
#define BAS_PERIOD_ALM0_INT 0
#define BAS_PERIOD_ALM1_INT 1
#define BAS_PERIOD_ALM2_INT 2
#define BAS_PERIOD_ALM3_INT 3
#define BAS_PERIOD_ALM4_INT 4
#define BAS_PERIOD_ALM5_INT 5
#define BAS_PERIOD_ALM6_INT 6
#define BAS_PERIOD_ALM7_INT 7
#define BAS_PERIOD_ALM8_INT 8
#define BAS_PERIOD_ALM9_INT 9
#define BAS_PERIOD_ALM10_INT 10
#define BAS_PERIOD_ALM11_INT 11
#define BAS_PERIOD_ALM12_INT 12
#define BAS_PERIOD_ALM13_INT 13
#define BAS_PERIOD_ALM14_INT 14
#define BAS_PERIOD_ALM15_INT 15
#define BAS_PERIOD_ALM0_INT_MASK 0
#define BAS_PERIOD_ALM1_INT_MASK 1
#define BAS_PERIOD_ALM2_INT_MASK 2
#define BAS_PERIOD_ALM3_INT_MASK 3
#define BAS_PERIOD_ALM4_INT_MASK 4
#define BAS_PERIOD_ALM5_INT_MASK 5
#define BAS_PERIOD_ALM6_INT_MASK 6
#define BAS_PERIOD_ALM7_INT_MASK 7
#define BAS_PERIOD_ALM8_INT_MASK 8
#define BAS_PERIOD_ALM9_INT_MASK 9
#define BAS_PERIOD_ALM10_INT_MASK 10
#define BAS_PERIOD_ALM11_INT_MASK 11
#define BAS_PERIOD_ALM12_INT_MASK 12
#define BAS_PERIOD_ALM13_INT_MASK 13
#define BAS_PERIOD_ALM14_INT_MASK 14
#define BAS_PERIOD_ALM15_INT_MASK 15
#define BAS_PERIOD_ALM16_INT 0
#define BAS_PERIOD_ALM17_INT 1
#define BAS_PERIOD_ALM18_INT 2
#define BAS_PERIOD_ALM19_INT 3
#define BAS_PERIOD_ALM20_INT 4
#define BAS_PERIOD_ALM21_INT 5
#define BAS_PERIOD_ALM22_INT 6
#define BAS_PERIOD_ALM23_INT 7
#define BAS_PERIOD_ALM24_INT 8
#define BAS_PERIOD_ALM25_INT 9
#define BAS_PERIOD_ALM26_INT 10
#define BAS_PERIOD_ALM27_INT 11
#define BAS_PERIOD_ALM28_INT 12
#define BAS_PERIOD_ALM29_INT 13
#define BAS_PERIOD_ALM30_INT 14
#define BAS_PERIOD_ALM31_INT 15
#define BAS_PERIOD_ALM16_INT_MASK 0
#define BAS_PERIOD_ALM17_INT_MASK 1
#define BAS_PERIOD_ALM18_INT_MASK 2
#define BAS_PERIOD_ALM19_INT_MASK 3
#define BAS_PERIOD_ALM20_INT_MASK 4
#define BAS_PERIOD_ALM21_INT_MASK 5
#define BAS_PERIOD_ALM22_INT_MASK 6
#define BAS_PERIOD_ALM23_INT_MASK 7
#define BAS_PERIOD_ALM24_INT_MASK 8
#define BAS_PERIOD_ALM25_INT_MASK 9
#define BAS_PERIOD_ALM26_INT_MASK 10
#define BAS_PERIOD_ALM27_INT_MASK 11
#define BAS_PERIOD_ALM28_INT_MASK 12
#define BAS_PERIOD_ALM29_INT_MASK 13
#define BAS_PERIOD_ALM30_INT_MASK 14
#define BAS_PERIOD_ALM31_INT_MASK 15
#define BAS_PERIOD_ALM32_INT 0
#define BAS_PERIOD_ALM33_INT 1
#define BAS_PERIOD_ALM34_INT 2
#define BAS_PERIOD_ALM35_INT 3
#define BAS_PERIOD_ALM36_INT 4
#define BAS_PERIOD_ALM37_INT 5
#define BAS_PERIOD_ALM38_INT 6
#define BAS_PERIOD_ALM39_INT 7
#define BAS_PERIOD_ALM40_INT 8
#define BAS_PERIOD_ALM41_INT 9
#define BAS_PERIOD_ALM42_INT 10
#define BAS_PERIOD_ALM43_INT 11
#define BAS_PERIOD_ALM44_INT 12
#define BAS_PERIOD_ALM45_INT 13
#define BAS_PERIOD_ALM46_INT 14
#define BAS_PERIOD_ALM47_INT 15
#define BAS_PERIOD_ALM32_INT_MASK 0
#define BAS_PERIOD_ALM33_INT_MASK 1
#define BAS_PERIOD_ALM34_INT_MASK 2
#define BAS_PERIOD_ALM35_INT_MASK 3
#define BAS_PERIOD_ALM36_INT_MASK 4
#define BAS_PERIOD_ALM37_INT_MASK 5
#define BAS_PERIOD_ALM38_INT_MASK 6
#define BAS_PERIOD_ALM39_INT_MASK 7
#define BAS_PERIOD_ALM40_INT_MASK 8
#define BAS_PERIOD_ALM41_INT_MASK 9
#define BAS_PERIOD_ALM42_INT_MASK 10
#define BAS_PERIOD_ALM43_INT_MASK 11
#define BAS_PERIOD_ALM44_INT_MASK 12
#define BAS_PERIOD_ALM45_INT_MASK 13
#define BAS_PERIOD_ALM46_INT_MASK 14
#define BAS_PERIOD_ALM47_INT_MASK 15
#define BAS_PERIOD_ALM48_INT 0
#define BAS_PERIOD_ALM49_INT 1
#define BAS_PERIOD_ALM50_INT 2
#define BAS_PERIOD_ALM51_INT 3
#define BAS_PERIOD_ALM52_INT 4
#define BAS_PERIOD_ALM53_INT 5
#define BAS_PERIOD_ALM54_INT 6
#define BAS_PERIOD_ALM55_INT 7
#define BAS_PERIOD_ALM56_INT 8
#define BAS_PERIOD_ALM57_INT 9
#define BAS_PERIOD_ALM58_INT 10
#define BAS_PERIOD_ALM59_INT 11
#define BAS_PERIOD_ALM60_INT 12
#define BAS_PERIOD_ALM61_INT 13
#define BAS_PERIOD_ALM62_INT 14
#define BAS_PERIOD_ALM63_INT 15
#define BAS_PERIOD_ALM48_INT_MASK 0
#define BAS_PERIOD_ALM49_INT_MASK 1
#define BAS_PERIOD_ALM50_INT_MASK 2
#define BAS_PERIOD_ALM51_INT_MASK 3
#define BAS_PERIOD_ALM52_INT_MASK 4
#define BAS_PERIOD_ALM53_INT_MASK 5
#define BAS_PERIOD_ALM54_INT_MASK 6
#define BAS_PERIOD_ALM55_INT_MASK 7
#define BAS_PERIOD_ALM56_INT_MASK 8
#define BAS_PERIOD_ALM57_INT_MASK 9
#define BAS_PERIOD_ALM58_INT_MASK 10
#define BAS_PERIOD_ALM59_INT_MASK 11
#define BAS_PERIOD_ALM60_INT_MASK 12
#define BAS_PERIOD_ALM61_INT_MASK 13
#define BAS_PERIOD_ALM62_INT_MASK 14
#define BAS_PERIOD_ALM63_INT_MASK 15
#define BAS_PERIOD_ALM64_INT 0
#define BAS_PERIOD_ALM65_INT 1
#define BAS_PERIOD_ALM66_INT 2
#define BAS_PERIOD_ALM67_INT 3
#define BAS_PERIOD_ALM68_INT 4
#define BAS_PERIOD_ALM69_INT 5
#define BAS_PERIOD_ALM70_INT 6
#define BAS_PERIOD_ALM71_INT 7
#define BAS_PERIOD_ALM72_INT 8
#define BAS_PERIOD_ALM73_INT 9
#define BAS_PERIOD_ALM74_INT 10
#define BAS_PERIOD_ALM75_INT 11
#define BAS_PERIOD_ALM76_INT 12
#define BAS_PERIOD_ALM77_INT 13
#define BAS_PERIOD_ALM78_INT 14
#define BAS_PERIOD_ALM79_INT 15
#define BAS_PERIOD_ALM64_INT_MASK 0
#define BAS_PERIOD_ALM65_INT_MASK 1
#define BAS_PERIOD_ALM66_INT_MASK 2
#define BAS_PERIOD_ALM67_INT_MASK 3
#define BAS_PERIOD_ALM68_INT_MASK 4
#define BAS_PERIOD_ALM69_INT_MASK 5
#define BAS_PERIOD_ALM70_INT_MASK 6
#define BAS_PERIOD_ALM71_INT_MASK 7
#define BAS_PERIOD_ALM72_INT_MASK 8
#define BAS_PERIOD_ALM73_INT_MASK 9
#define BAS_PERIOD_ALM74_INT_MASK 10
#define BAS_PERIOD_ALM75_INT_MASK 11
#define BAS_PERIOD_ALM76_INT_MASK 12
#define BAS_PERIOD_ALM77_INT_MASK 13
#define BAS_PERIOD_ALM78_INT_MASK 14
#define BAS_PERIOD_ALM79_INT_MASK 15
#define RX_SFBEI_0_INT 0
#define RX_SFBEI_1_INT 1
#define RX_SFBEI_2_INT 2
#define RX_SFBEI_3_INT 3
#define RX_SFBEI_4_INT 4
#define RX_SFBEI_5_INT 5
#define RX_SFBEI_6_INT 6
#define RX_SFBEI_7_INT 7
#define RX_SFBEI_8_INT 8
#define RX_SFBEI_9_INT 9
#define RX_SFBEI_10_INT 10
#define RX_SFBEI_11_INT 11
#define RX_SFBEI_12_INT 12
#define RX_SFBEI_13_INT 13
#define RX_SFBEI_14_INT 14
#define RX_SFBEI_15_INT 15
#define RX_SFBEI_0_INT_MASK 0
#define RX_SFBEI_1_INT_MASK 1
#define RX_SFBEI_2_INT_MASK 2
#define RX_SFBEI_3_INT_MASK 3
#define RX_SFBEI_4_INT_MASK 4
#define RX_SFBEI_5_INT_MASK 5
#define RX_SFBEI_6_INT_MASK 6
#define RX_SFBEI_7_INT_MASK 7
#define RX_SFBEI_8_INT_MASK 8
#define RX_SFBEI_9_INT_MASK 9
#define RX_SFBEI_10_INT_MASK 10
#define RX_SFBEI_11_INT_MASK 11
#define RX_SFBEI_12_INT_MASK 12
#define RX_SFBEI_13_INT_MASK 13
#define RX_SFBEI_14_INT_MASK 14
#define RX_SFBEI_15_INT_MASK 15
#define RX_SFBIP_0_INT 0
#define RX_SFBIP_1_INT 1
#define RX_SFBIP_2_INT 2
#define RX_SFBIP_3_INT 3
#define RX_SFBIP_4_INT 4
#define RX_SFBIP_5_INT 5
#define RX_SFBIP_6_INT 6
#define RX_SFBIP_7_INT 7
#define RX_SFBIP_8_INT 8
#define RX_SFBIP_9_INT 9
#define RX_SFBIP_10_INT 10
#define RX_SFBIP_11_INT 11
#define RX_SFBIP_12_INT 12
#define RX_SFBIP_13_INT 13
#define RX_SFBIP_14_INT 14
#define RX_SFBIP_15_INT 15
#define RX_SFBIP_0_INT_MASK 0
#define RX_SFBIP_1_INT_MASK 1
#define RX_SFBIP_2_INT_MASK 2
#define RX_SFBIP_3_INT_MASK 3
#define RX_SFBIP_4_INT_MASK 4
#define RX_SFBIP_5_INT_MASK 5
#define RX_SFBIP_6_INT_MASK 6
#define RX_SFBIP_7_INT_MASK 7
#define RX_SFBIP_8_INT_MASK 8
#define RX_SFBIP_9_INT_MASK 9
#define RX_SFBIP_10_INT_MASK 10
#define RX_SFBIP_11_INT_MASK 11
#define RX_SFBIP_12_INT_MASK 12
#define RX_SFBIP_13_INT_MASK 13
#define RX_SFBIP_14_INT_MASK 14
#define RX_SFBIP_15_INT_MASK 15
#define RX_SFBIP_16_INT 0
#define RX_SFBIP_17_INT 1
#define RX_SFBIP_18_INT 2
#define RX_SFBIP_19_INT 3
#define RX_SFBIP_20_INT 4
#define RX_SFBIP_21_INT 5
#define RX_SFBIP_22_INT 6
#define RX_SFBIP_23_INT 7
#define RX_SFBIP_24_INT 8
#define RX_SFBIP_25_INT 9
#define RX_SFBIP_26_INT 10
#define RX_SFBIP_27_INT 11
#define RX_SFBIP_28_INT 12
#define RX_SFBIP_29_INT 13
#define RX_SFBIP_30_INT 14
#define RX_SFBIP_31_INT 15
#define RX_SFBIP_16_INT_MASK 0
#define RX_SFBIP_17_INT_MASK 1
#define RX_SFBIP_18_INT_MASK 2
#define RX_SFBIP_19_INT_MASK 3
#define RX_SFBIP_20_INT_MASK 4
#define RX_SFBIP_21_INT_MASK 5
#define RX_SFBIP_22_INT_MASK 6
#define RX_SFBIP_23_INT_MASK 7
#define RX_SFBIP_24_INT_MASK 8
#define RX_SFBIP_25_INT_MASK 9
#define RX_SFBIP_26_INT_MASK 10
#define RX_SFBIP_27_INT_MASK 11
#define RX_SFBIP_28_INT_MASK 12
#define RX_SFBIP_29_INT_MASK 13
#define RX_SFBIP_30_INT_MASK 14
#define RX_SFBIP_31_INT_MASK 15
#define RX_SFBIP_32_INT 0
#define RX_SFBIP_33_INT 1
#define RX_SFBIP_34_INT 2
#define RX_SFBIP_35_INT 3
#define RX_SFBIP_36_INT 4
#define RX_SFBIP_37_INT 5
#define RX_SFBIP_38_INT 6
#define RX_SFBIP_39_INT 7
#define RX_SFBIP_40_INT 8
#define RX_SFBIP_41_INT 9
#define RX_SFBIP_42_INT 10
#define RX_SFBIP_43_INT 11
#define RX_SFBIP_44_INT 12
#define RX_SFBIP_45_INT 13
#define RX_SFBIP_46_INT 14
#define RX_SFBIP_47_INT 15
#define RX_SFBIP_32_INT_MASK 0
#define RX_SFBIP_33_INT_MASK 1
#define RX_SFBIP_34_INT_MASK 2
#define RX_SFBIP_35_INT_MASK 3
#define RX_SFBIP_36_INT_MASK 4
#define RX_SFBIP_37_INT_MASK 5
#define RX_SFBIP_38_INT_MASK 6
#define RX_SFBIP_39_INT_MASK 7
#define RX_SFBIP_40_INT_MASK 8
#define RX_SFBIP_41_INT_MASK 9
#define RX_SFBIP_42_INT_MASK 10
#define RX_SFBIP_43_INT_MASK 11
#define RX_SFBIP_44_INT_MASK 12
#define RX_SFBIP_45_INT_MASK 13
#define RX_SFBIP_46_INT_MASK 14
#define RX_SFBIP_47_INT_MASK 15
#define RX_SFBIP_48_INT 0
#define RX_SFBIP_49_INT 1
#define RX_SFBIP_50_INT 2
#define RX_SFBIP_51_INT 3
#define RX_SFBIP_52_INT 4
#define RX_SFBIP_53_INT 5
#define RX_SFBIP_54_INT 6
#define RX_SFBIP_55_INT 7
#define RX_SFBIP_56_INT 8
#define RX_SFBIP_57_INT 9
#define RX_SFBIP_58_INT 10
#define RX_SFBIP_59_INT 11
#define RX_SFBIP_60_INT 12
#define RX_SFBIP_61_INT 13
#define RX_SFBIP_62_INT 14
#define RX_SFBIP_63_INT 15
#define RX_SFBIP_48_INT_MASK 0
#define RX_SFBIP_49_INT_MASK 1
#define RX_SFBIP_50_INT_MASK 2
#define RX_SFBIP_51_INT_MASK 3
#define RX_SFBIP_52_INT_MASK 4
#define RX_SFBIP_53_INT_MASK 5
#define RX_SFBIP_54_INT_MASK 6
#define RX_SFBIP_55_INT_MASK 7
#define RX_SFBIP_56_INT_MASK 8
#define RX_SFBIP_57_INT_MASK 9
#define RX_SFBIP_58_INT_MASK 10
#define RX_SFBIP_59_INT_MASK 11
#define RX_SFBIP_60_INT_MASK 12
#define RX_SFBIP_61_INT_MASK 13
#define RX_SFBIP_62_INT_MASK 14
#define RX_SFBIP_63_INT_MASK 15
#define RX_SFBIP_64_INT 0
#define RX_SFBIP_65_INT 1
#define RX_SFBIP_66_INT 2
#define RX_SFBIP_67_INT 3
#define RX_SFBIP_68_INT 4
#define RX_SFBIP_69_INT 5
#define RX_SFBIP_70_INT 6
#define RX_SFBIP_71_INT 7
#define RX_SFBIP_72_INT 8
#define RX_SFBIP_73_INT 9
#define RX_SFBIP_74_INT 10
#define RX_SFBIP_75_INT 11
#define RX_SFBIP_76_INT 12
#define RX_SFBIP_77_INT 13
#define RX_SFBIP_78_INT 14
#define RX_SFBIP_79_INT 15
#define RX_SFBIP_64_INT_MASK 0
#define RX_SFBIP_65_INT_MASK 1
#define RX_SFBIP_66_INT_MASK 2
#define RX_SFBIP_67_INT_MASK 3
#define RX_SFBIP_68_INT_MASK 4
#define RX_SFBIP_69_INT_MASK 5
#define RX_SFBIP_70_INT_MASK 6
#define RX_SFBIP_71_INT_MASK 7
#define RX_SFBIP_72_INT_MASK 8
#define RX_SFBIP_73_INT_MASK 9
#define RX_SFBIP_74_INT_MASK 10
#define RX_SFBIP_75_INT_MASK 11
#define RX_SFBIP_76_INT_MASK 12
#define RX_SFBIP_77_INT_MASK 13
#define RX_SFBIP_78_INT_MASK 14
#define RX_SFBIP_79_INT_MASK 15
#define RX_SFBEI_16_INT 0
#define RX_SFBEI_17_INT 1
#define RX_SFBEI_18_INT 2
#define RX_SFBEI_19_INT 3
#define RX_SFBEI_20_INT 4
#define RX_SFBEI_21_INT 5
#define RX_SFBEI_22_INT 6
#define RX_SFBEI_23_INT 7
#define RX_SFBEI_24_INT 8
#define RX_SFBEI_25_INT 9
#define RX_SFBEI_26_INT 10
#define RX_SFBEI_27_INT 11
#define RX_SFBEI_28_INT 12
#define RX_SFBEI_29_INT 13
#define RX_SFBEI_30_INT 14
#define RX_SFBEI_31_INT 15
#define RX_SFBEI_16_INT_MASK 0
#define RX_SFBEI_17_INT_MASK 1
#define RX_SFBEI_18_INT_MASK 2
#define RX_SFBEI_19_INT_MASK 3
#define RX_SFBEI_20_INT_MASK 4
#define RX_SFBEI_21_INT_MASK 5
#define RX_SFBEI_22_INT_MASK 6
#define RX_SFBEI_23_INT_MASK 7
#define RX_SFBEI_24_INT_MASK 8
#define RX_SFBEI_25_INT_MASK 9
#define RX_SFBEI_26_INT_MASK 10
#define RX_SFBEI_27_INT_MASK 11
#define RX_SFBEI_28_INT_MASK 12
#define RX_SFBEI_29_INT_MASK 13
#define RX_SFBEI_30_INT_MASK 14
#define RX_SFBEI_31_INT_MASK 15
#define RX_SFBEI_32_INT 0
#define RX_SFBEI_33_INT 1
#define RX_SFBEI_34_INT 2
#define RX_SFBEI_35_INT 3
#define RX_SFBEI_36_INT 4
#define RX_SFBEI_37_INT 5
#define RX_SFBEI_38_INT 6
#define RX_SFBEI_39_INT 7
#define RX_SFBEI_40_INT 8
#define RX_SFBEI_41_INT 9
#define RX_SFBEI_42_INT 10
#define RX_SFBEI_43_INT 11
#define RX_SFBEI_44_INT 12
#define RX_SFBEI_45_INT 13
#define RX_SFBEI_46_INT 14
#define RX_SFBEI_47_INT 15
#define RX_SFBEI_32_INT_MASK 0
#define RX_SFBEI_33_INT_MASK 1
#define RX_SFBEI_34_INT_MASK 2
#define RX_SFBEI_35_INT_MASK 3
#define RX_SFBEI_36_INT_MASK 4
#define RX_SFBEI_37_INT_MASK 5
#define RX_SFBEI_38_INT_MASK 6
#define RX_SFBEI_39_INT_MASK 7
#define RX_SFBEI_40_INT_MASK 8
#define RX_SFBEI_41_INT_MASK 9
#define RX_SFBEI_42_INT_MASK 10
#define RX_SFBEI_43_INT_MASK 11
#define RX_SFBEI_44_INT_MASK 12
#define RX_SFBEI_45_INT_MASK 13
#define RX_SFBEI_46_INT_MASK 14
#define RX_SFBEI_47_INT_MASK 15
#define RX_SFBEI_48_INT 0
#define RX_SFBEI_49_INT 1
#define RX_SFBEI_50_INT 2
#define RX_SFBEI_51_INT 3
#define RX_SFBEI_52_INT 4
#define RX_SFBEI_53_INT 5
#define RX_SFBEI_54_INT 6
#define RX_SFBEI_55_INT 7
#define RX_SFBEI_56_INT 8
#define RX_SFBEI_57_INT 9
#define RX_SFBEI_58_INT 10
#define RX_SFBEI_59_INT 11
#define RX_SFBEI_60_INT 12
#define RX_SFBEI_61_INT 13
#define RX_SFBEI_62_INT 14
#define RX_SFBEI_63_INT 15
#define RX_SFBEI_48_INT_MASK 0
#define RX_SFBEI_49_INT_MASK 1
#define RX_SFBEI_50_INT_MASK 2
#define RX_SFBEI_51_INT_MASK 3
#define RX_SFBEI_52_INT_MASK 4
#define RX_SFBEI_53_INT_MASK 5
#define RX_SFBEI_54_INT_MASK 6
#define RX_SFBEI_55_INT_MASK 7
#define RX_SFBEI_56_INT_MASK 8
#define RX_SFBEI_57_INT_MASK 9
#define RX_SFBEI_58_INT_MASK 10
#define RX_SFBEI_59_INT_MASK 11
#define RX_SFBEI_60_INT_MASK 12
#define RX_SFBEI_61_INT_MASK 13
#define RX_SFBEI_62_INT_MASK 14
#define RX_SFBEI_63_INT_MASK 15
#define RX_SFBEI_64_INT 0
#define RX_SFBEI_65_INT 1
#define RX_SFBEI_66_INT 2
#define RX_SFBEI_67_INT 3
#define RX_SFBEI_68_INT 4
#define RX_SFBEI_69_INT 5
#define RX_SFBEI_70_INT 6
#define RX_SFBEI_71_INT 7
#define RX_SFBEI_72_INT 8
#define RX_SFBEI_73_INT 9
#define RX_SFBEI_74_INT 10
#define RX_SFBEI_75_INT 11
#define RX_SFBEI_76_INT 12
#define RX_SFBEI_77_INT 13
#define RX_SFBEI_78_INT 14
#define RX_SFBEI_79_INT 15
#define RX_SFBEI_64_INT_MASK 0
#define RX_SFBEI_65_INT_MASK 1
#define RX_SFBEI_66_INT_MASK 2
#define RX_SFBEI_67_INT_MASK 3
#define RX_SFBEI_68_INT_MASK 4
#define RX_SFBEI_69_INT_MASK 5
#define RX_SFBEI_70_INT_MASK 6
#define RX_SFBEI_71_INT_MASK 7
#define RX_SFBEI_72_INT_MASK 8
#define RX_SFBEI_73_INT_MASK 9
#define RX_SFBEI_74_INT_MASK 10
#define RX_SFBEI_75_INT_MASK 11
#define RX_SFBEI_76_INT_MASK 12
#define RX_SFBEI_77_INT_MASK 13
#define RX_SFBEI_78_INT_MASK 14
#define RX_SFBEI_79_INT_MASK 15

/******************************************************************************
*
* MOUDLE_NAME
*
*	 oam_tx
*
* MOUDEL_ADDRESS
*
*	 oam_tx
*
* MOUDLE_OFFSET
*
*    oam_tx
*
* MOUDLE_RegisterName
*
* 	oam_tx
*
*
******************************************************************************/
/*****************oam_tx****************/
#define BAS_BRK_CFG 0
#define LPI_INSERT_EN 1
#define LF_INSERT_EN 2
#define RF_INSERT_EN 3
#define BAS_BRK_CH_CFG 4
#define TS_1DM_INSERT_EN 5
#define TS_2DMM_INSERT_EN 6
#define TS_2DMR_INSERT_EN 7
#define TS_DM_SEL 8
#define FAULT_RPL_EN 9
#define BAS_IDTY_CFG 0
#define BAS_PERIOD_CFG_0 0
#define BAS_PERIOD_CFG_1 1
#define BAS_PERIOD_CFG_2 2
#define BAS_PERIOD_CFG_3 3
#define BAS_PERIOD_CFG_4 4
#define SEQ_TYPE_CFG_0 0
#define SEQ_TYPE_CFG_1 1
#define SEQ_TYPE_CFG_2 2
#define SEQ_NUM_CFG_0 0
#define SEQ_NUM_CFG_1 1
#define SEQ_NUM_CFG_2 2
#define SEQ_NUM_CFG_3 3
#define SEQ_NUM_CFG_4 4
#define SEQ_NUM_CFG_5 5
#define SEQ_NUM_CFG_6 6
#define SEQ_NUM_CFG_7 7
#define SEQ_NUM_CFG_8 8
#define SEQ_NUM_CFG_9 9
#define CRC_REV_CFG 0
#define OTH_CRC_REV_CFG 1
#define TX_IDLE_RPL_EN_0 0
#define TX_IDLE_RPL_EN_1 1
#define TX_IDLE_RPL_EN_2 2
#define OAM_TX_BYPASS 0
#define BIP_ERR_MASK 0
#define MANUAL_INSERT_CSF_LPI_0 0
#define MANUAL_INSERT_CSF_LPI_1 1
#define MANUAL_INSERT_CSF_LPI_2 2
#define MANUAL_INSERT_CSF_LF_0 0
#define MANUAL_INSERT_CSF_LF_1 1
#define MANUAL_INSERT_CSF_LF_2 2
#define MANUAL_INSERT_CSF_RF_0 0
#define MANUAL_INSERT_CSF_RF_1 1
#define MANUAL_INSERT_CSF_RF_2 2
#define BIP_ERR_LEVEL_0 0
#define BIP_ERR_LEVEL_1 1
#define BIP_ERR_LEVEL_2 2
#define OAM_TX_BYPASS_CLIENT_0 0
#define OAM_TX_BYPASS_CLIENT_1 1
#define OAM_TX_BYPASS_CLIENT_2 2
#define MANUAL_INSERT_RDI_0 0
#define MANUAL_INSERT_RDI_1 1
#define MANUAL_INSERT_RDI_2 2
#define BIP_ERR_PULSE_0 0
#define BIP_ERR_PULSE_1 1
#define BIP_ERR_PULSE_2 2
#define BAS_INSERT_CNT_0 0
#define BAS_INSERT_CNT_1 1
#define BAS_INSERT_CNT_2 2
#define BAS_INSERT_CNT_3 3
#define BAS_INSERT_CNT_4 0
#define BAS_INSERT_CNT_5 1
#define BAS_INSERT_CNT_6 2
#define BAS_INSERT_CNT_7 3
#define BAS_INSERT_CNT_8 0
#define BAS_INSERT_CNT_9 1
#define BAS_INSERT_CNT_10 2
#define BAS_INSERT_CNT_11 3
#define BAS_INSERT_CNT_12 0
#define BAS_INSERT_CNT_13 1
#define BAS_INSERT_CNT_14 2
#define BAS_INSERT_CNT_15 3
#define BAS_INSERT_CNT_16 0
#define BAS_INSERT_CNT_17 1
#define BAS_INSERT_CNT_18 2
#define BAS_INSERT_CNT_19 3
#define BAS_INSERT_CNT_20 0
#define BAS_INSERT_CNT_21 1
#define BAS_INSERT_CNT_22 2
#define BAS_INSERT_CNT_23 3
#define BAS_INSERT_CNT_24 0
#define BAS_INSERT_CNT_25 1
#define BAS_INSERT_CNT_26 2
#define BAS_INSERT_CNT_27 3
#define BAS_INSERT_CNT_28 0
#define BAS_INSERT_CNT_29 1
#define BAS_INSERT_CNT_30 2
#define BAS_INSERT_CNT_31 3
#define BAS_INSERT_CNT_32 0
#define BAS_INSERT_CNT_33 1
#define BAS_INSERT_CNT_34 2
#define BAS_INSERT_CNT_35 3
#define BAS_INSERT_CNT_36 0
#define BAS_INSERT_CNT_37 1
#define BAS_INSERT_CNT_38 2
#define BAS_INSERT_CNT_39 3
#define BAS_INSERT_CNT_40 0
#define BAS_INSERT_CNT_41 1
#define BAS_INSERT_CNT_42 2
#define BAS_INSERT_CNT_43 3
#define BAS_INSERT_CNT_44 0
#define BAS_INSERT_CNT_45 1
#define BAS_INSERT_CNT_46 2
#define BAS_INSERT_CNT_47 3
#define BAS_INSERT_CNT_48 0
#define BAS_INSERT_CNT_49 1
#define BAS_INSERT_CNT_50 2
#define BAS_INSERT_CNT_51 3
#define BAS_INSERT_CNT_52 0
#define BAS_INSERT_CNT_53 1
#define BAS_INSERT_CNT_54 2
#define BAS_INSERT_CNT_55 3
#define BAS_INSERT_CNT_56 0
#define BAS_INSERT_CNT_57 1
#define BAS_INSERT_CNT_58 2
#define BAS_INSERT_CNT_59 3
#define BAS_INSERT_CNT_60 0
#define BAS_INSERT_CNT_61 1
#define BAS_INSERT_CNT_62 2
#define BAS_INSERT_CNT_63 3
#define BAS_INSERT_CNT_64 0
#define BAS_INSERT_CNT_65 1
#define BAS_INSERT_CNT_66 2
#define BAS_INSERT_CNT_67 3
#define BAS_INSERT_CNT_68 0
#define BAS_INSERT_CNT_69 1
#define BAS_INSERT_CNT_70 2
#define BAS_INSERT_CNT_71 3
#define BAS_INSERT_CNT_72 0
#define BAS_INSERT_CNT_73 1
#define BAS_INSERT_CNT_74 2
#define BAS_INSERT_CNT_75 3
#define BAS_INSERT_CNT_76 0
#define BAS_INSERT_CNT_77 1
#define BAS_INSERT_CNT_78 2
#define BAS_INSERT_CNT_79 3
#define OTH_INSERT_CNT_0 0
#define OTH_INSERT_CNT_1 1
#define OTH_INSERT_CNT_2 2
#define OTH_INSERT_CNT_3 3
#define OTH_INSERT_CNT_4 0
#define OTH_INSERT_CNT_5 1
#define OTH_INSERT_CNT_6 2
#define OTH_INSERT_CNT_7 3
#define OTH_INSERT_CNT_8 0
#define OTH_INSERT_CNT_9 1
#define OTH_INSERT_CNT_10 2
#define OTH_INSERT_CNT_11 3
#define OTH_INSERT_CNT_12 0
#define OTH_INSERT_CNT_13 1
#define OTH_INSERT_CNT_14 2
#define OTH_INSERT_CNT_15 3
#define OTH_INSERT_CNT_16 0
#define OTH_INSERT_CNT_17 1
#define OTH_INSERT_CNT_18 2
#define OTH_INSERT_CNT_19 3
#define OTH_INSERT_CNT_20 0
#define OTH_INSERT_CNT_21 1
#define OTH_INSERT_CNT_22 2
#define OTH_INSERT_CNT_23 3
#define OTH_INSERT_CNT_24 0
#define OTH_INSERT_CNT_25 1
#define OTH_INSERT_CNT_26 2
#define OTH_INSERT_CNT_27 3
#define OTH_INSERT_CNT_28 0
#define OTH_INSERT_CNT_29 1
#define OTH_INSERT_CNT_30 2
#define OTH_INSERT_CNT_31 3
#define OTH_INSERT_CNT_32 0
#define OTH_INSERT_CNT_33 1
#define OTH_INSERT_CNT_34 2
#define OTH_INSERT_CNT_35 3
#define OTH_INSERT_CNT_36 0
#define OTH_INSERT_CNT_37 1
#define OTH_INSERT_CNT_38 2
#define OTH_INSERT_CNT_39 3
#define OTH_INSERT_CNT_40 0
#define OTH_INSERT_CNT_41 1
#define OTH_INSERT_CNT_42 2
#define OTH_INSERT_CNT_43 3
#define OTH_INSERT_CNT_44 0
#define OTH_INSERT_CNT_45 1
#define OTH_INSERT_CNT_46 2
#define OTH_INSERT_CNT_47 3
#define OTH_INSERT_CNT_48 0
#define OTH_INSERT_CNT_49 1
#define OTH_INSERT_CNT_50 2
#define OTH_INSERT_CNT_51 3
#define OTH_INSERT_CNT_52 0
#define OTH_INSERT_CNT_53 1
#define OTH_INSERT_CNT_54 2
#define OTH_INSERT_CNT_55 3
#define OTH_INSERT_CNT_56 0
#define OTH_INSERT_CNT_57 1
#define OTH_INSERT_CNT_58 2
#define OTH_INSERT_CNT_59 3
#define OTH_INSERT_CNT_60 0
#define OTH_INSERT_CNT_61 1
#define OTH_INSERT_CNT_62 2
#define OTH_INSERT_CNT_63 3
#define OTH_INSERT_CNT_64 0
#define OTH_INSERT_CNT_65 1
#define OTH_INSERT_CNT_66 2
#define OTH_INSERT_CNT_67 3
#define OTH_INSERT_CNT_68 0
#define OTH_INSERT_CNT_69 1
#define OTH_INSERT_CNT_70 2
#define OTH_INSERT_CNT_71 3
#define OTH_INSERT_CNT_72 0
#define OTH_INSERT_CNT_73 1
#define OTH_INSERT_CNT_74 2
#define OTH_INSERT_CNT_75 3
#define OTH_INSERT_CNT_76 0
#define OTH_INSERT_CNT_77 1
#define OTH_INSERT_CNT_78 2
#define OTH_INSERT_CNT_79 3
#define CSF_LPI_0 0
#define CSF_LPI_1 1
#define CSF_LPI_2 2
#define CSF_LF_0 0
#define CSF_LF_1 1
#define CSF_LF_2 2
#define CSF_RF_0 0
#define CSF_RF_1 1
#define CSF_RF_2 2
#define FIFO_FULL_REI_0 0
#define FIFO_FULL_REI_1 1
#define FIFO_FULL_REI_2 2
#define FIFO_FULL_APS_0 0
#define FIFO_FULL_APS_1 1
#define FIFO_FULL_APS_2 2
#define FIFO_FULL_LOW_0 0
#define FIFO_FULL_LOW_1 1
#define FIFO_FULL_LOW_2 2
#define DATA_ERR_PULSE_0 0
#define DATA_ERR_PULSE_1 1
#define DATA_ERR_PULSE_2 2
#define BAS_BRK_CNT 0
#define BAS_CNT 0
#define APS_CNT 0
#define LOW_CNT 0
#define NEW_CNT 0
#define TIMESTAMP_LOCK_0 0
#define TIMESTAMP_LOCK_1 1
#define BAS_PERIOD_CFG0 0
#define BAS_PERIOD_CFG1 1
#define BAS_PERIOD_CFG2 0
#define BAS_PERIOD_CFG3 1
#define BAS_EN_CFG_ASIC_0 0
#define BAS_EN_CFG_ASIC_1 1
#define BAS_EN_CFG_ASIC_2 2
#define OAM_PRI_ENCODE 0
#define BAS_SOURCE_SEL 1
#define TX_OHIF_FIFO_SEL_0 0
#define TX_OHIF_FIFO_SEL_1 1
#define TX_OHIF_FIFO_SEL_2 2
#define TX_OHIF_FIFO_SEL_3 3
#define NEW_PERIOD_CFG 0
#define APS_PERIOD_CFG 1
#define TX_BAS_FIFO_NUM 0
#define LPI_PRE_CNT 0
#define LPI_POS_CNT 1
#define OAM_TX_INSERT_66BCODE_CFG_0 0
#define OAM_TX_INSERT_66BCODE_CFG_1 1
#define OAM_TX_INSERT_66BCODE_CFG_2 2
#define OAM_JOIT_BIP_CFG_0 0
#define OAM_JOIT_BIP_CFG_1 1
#define TX_LPI_BAS_INS_EN_0 0
#define TX_LPI_BAS_INS_EN_1 1
#define TX_LPI_BAS_INS_EN_2 2
#define TX_BAS_CRC_ERR_EN_0 0
#define TX_BAS_CRC_ERR_EN_1 1
#define TX_BAS_CRC_ERR_EN_2 2
#define TX_BAS_CRC_ERR_MASK 3
#define OAM_CNT_CH_CFG 0
#define TX_OAM_CNT_CLR 1
#define TX_LPI_IDLE_RPL_EN_0 0
#define TX_LPI_IDLE_RPL_EN_1 1
#define TX_LPI_IDLE_RPL_EN_2 2
#define TX_INSERT_66BCODE_EN_0 0
#define TX_INSERT_66BCODE_EN_1 1
#define TX_INSERT_66BCODE_EN_2 2
#define FIFO_FULL_BAS_0 0
#define FIFO_FULL_BAS_1 1
#define FIFO_FULL_BAS_2 2
#define FIFO_FULL_NEW_0 0
#define FIFO_FULL_NEW_1 1
#define FIFO_FULL_NEW_2 2
#define FIFO_EMPTY_BAS_0 0
#define FIFO_EMPTY_BAS_1 1
#define FIFO_EMPTY_BAS_2 2
#define CSF_LPI_0_INT 0
#define CSF_LPI_1_INT 1
#define CSF_LPI_2_INT 2
#define CSF_LPI_3_INT 3
#define CSF_LPI_4_INT 4
#define CSF_LPI_5_INT 5
#define CSF_LPI_6_INT 6
#define CSF_LPI_7_INT 7
#define CSF_LPI_8_INT 8
#define CSF_LPI_9_INT 9
#define CSF_LPI_0_INT_MASK 0
#define CSF_LPI_1_INT_MASK 1
#define CSF_LPI_2_INT_MASK 2
#define CSF_LPI_3_INT_MASK 3
#define CSF_LPI_4_INT_MASK 4
#define CSF_LPI_5_INT_MASK 5
#define CSF_LPI_6_INT_MASK 6
#define CSF_LPI_7_INT_MASK 7
#define CSF_LPI_8_INT_MASK 8
#define CSF_LPI_9_INT_MASK 9
#define CSF_LPI_10_INT 0
#define CSF_LPI_11_INT 1
#define CSF_LPI_12_INT 2
#define CSF_LPI_13_INT 3
#define CSF_LPI_14_INT 4
#define CSF_LPI_15_INT 5
#define CSF_LPI_16_INT 6
#define CSF_LPI_17_INT 7
#define CSF_LPI_18_INT 8
#define CSF_LPI_19_INT 9
#define CSF_LPI_10_INT_MASK 0
#define CSF_LPI_11_INT_MASK 1
#define CSF_LPI_12_INT_MASK 2
#define CSF_LPI_13_INT_MASK 3
#define CSF_LPI_14_INT_MASK 4
#define CSF_LPI_15_INT_MASK 5
#define CSF_LPI_16_INT_MASK 6
#define CSF_LPI_17_INT_MASK 7
#define CSF_LPI_18_INT_MASK 8
#define CSF_LPI_19_INT_MASK 9
#define CSF_LPI_20_INT 0
#define CSF_LPI_21_INT 1
#define CSF_LPI_22_INT 2
#define CSF_LPI_23_INT 3
#define CSF_LPI_24_INT 4
#define CSF_LPI_25_INT 5
#define CSF_LPI_26_INT 6
#define CSF_LPI_27_INT 7
#define CSF_LPI_28_INT 8
#define CSF_LPI_29_INT 9
#define CSF_LPI_20_INT_MASK 0
#define CSF_LPI_21_INT_MASK 1
#define CSF_LPI_22_INT_MASK 2
#define CSF_LPI_23_INT_MASK 3
#define CSF_LPI_24_INT_MASK 4
#define CSF_LPI_25_INT_MASK 5
#define CSF_LPI_26_INT_MASK 6
#define CSF_LPI_27_INT_MASK 7
#define CSF_LPI_28_INT_MASK 8
#define CSF_LPI_29_INT_MASK 9
#define CSF_LPI_30_INT 0
#define CSF_LPI_31_INT 1
#define CSF_LPI_32_INT 2
#define CSF_LPI_33_INT 3
#define CSF_LPI_34_INT 4
#define CSF_LPI_35_INT 5
#define CSF_LPI_36_INT 6
#define CSF_LPI_37_INT 7
#define CSF_LPI_38_INT 8
#define CSF_LPI_39_INT 9
#define CSF_LPI_30_INT_MASK 0
#define CSF_LPI_31_INT_MASK 1
#define CSF_LPI_32_INT_MASK 2
#define CSF_LPI_33_INT_MASK 3
#define CSF_LPI_34_INT_MASK 4
#define CSF_LPI_35_INT_MASK 5
#define CSF_LPI_36_INT_MASK 6
#define CSF_LPI_37_INT_MASK 7
#define CSF_LPI_38_INT_MASK 8
#define CSF_LPI_39_INT_MASK 9
#define CSF_LPI_40_INT 0
#define CSF_LPI_41_INT 1
#define CSF_LPI_42_INT 2
#define CSF_LPI_43_INT 3
#define CSF_LPI_44_INT 4
#define CSF_LPI_45_INT 5
#define CSF_LPI_46_INT 6
#define CSF_LPI_47_INT 7
#define CSF_LPI_48_INT 8
#define CSF_LPI_49_INT 9
#define CSF_LPI_40_INT_MASK 0
#define CSF_LPI_41_INT_MASK 1
#define CSF_LPI_42_INT_MASK 2
#define CSF_LPI_43_INT_MASK 3
#define CSF_LPI_44_INT_MASK 4
#define CSF_LPI_45_INT_MASK 5
#define CSF_LPI_46_INT_MASK 6
#define CSF_LPI_47_INT_MASK 7
#define CSF_LPI_48_INT_MASK 8
#define CSF_LPI_49_INT_MASK 9
#define CSF_LPI_50_INT 0
#define CSF_LPI_51_INT 1
#define CSF_LPI_52_INT 2
#define CSF_LPI_53_INT 3
#define CSF_LPI_54_INT 4
#define CSF_LPI_55_INT 5
#define CSF_LPI_56_INT 6
#define CSF_LPI_57_INT 7
#define CSF_LPI_58_INT 8
#define CSF_LPI_59_INT 9
#define CSF_LPI_50_INT_MASK 0
#define CSF_LPI_51_INT_MASK 1
#define CSF_LPI_52_INT_MASK 2
#define CSF_LPI_53_INT_MASK 3
#define CSF_LPI_54_INT_MASK 4
#define CSF_LPI_55_INT_MASK 5
#define CSF_LPI_56_INT_MASK 6
#define CSF_LPI_57_INT_MASK 7
#define CSF_LPI_58_INT_MASK 8
#define CSF_LPI_59_INT_MASK 9
#define CSF_LPI_60_INT 0
#define CSF_LPI_61_INT 1
#define CSF_LPI_62_INT 2
#define CSF_LPI_63_INT 3
#define CSF_LPI_64_INT 4
#define CSF_LPI_65_INT 5
#define CSF_LPI_66_INT 6
#define CSF_LPI_67_INT 7
#define CSF_LPI_68_INT 8
#define CSF_LPI_69_INT 9
#define CSF_LPI_60_INT_MASK 0
#define CSF_LPI_61_INT_MASK 1
#define CSF_LPI_62_INT_MASK 2
#define CSF_LPI_63_INT_MASK 3
#define CSF_LPI_64_INT_MASK 4
#define CSF_LPI_65_INT_MASK 5
#define CSF_LPI_66_INT_MASK 6
#define CSF_LPI_67_INT_MASK 7
#define CSF_LPI_68_INT_MASK 8
#define CSF_LPI_69_INT_MASK 9
#define CSF_LPI_70_INT 0
#define CSF_LPI_71_INT 1
#define CSF_LPI_72_INT 2
#define CSF_LPI_73_INT 3
#define CSF_LPI_74_INT 4
#define CSF_LPI_75_INT 5
#define CSF_LPI_76_INT 6
#define CSF_LPI_77_INT 7
#define CSF_LPI_78_INT 8
#define CSF_LPI_79_INT 9
#define CSF_LPI_70_INT_MASK 0
#define CSF_LPI_71_INT_MASK 1
#define CSF_LPI_72_INT_MASK 2
#define CSF_LPI_73_INT_MASK 3
#define CSF_LPI_74_INT_MASK 4
#define CSF_LPI_75_INT_MASK 5
#define CSF_LPI_76_INT_MASK 6
#define CSF_LPI_77_INT_MASK 7
#define CSF_LPI_78_INT_MASK 8
#define CSF_LPI_79_INT_MASK 9
#define CSF_LF_0_INT 0
#define CSF_LF_1_INT 1
#define CSF_LF_2_INT 2
#define CSF_LF_3_INT 3
#define CSF_LF_4_INT 4
#define CSF_LF_5_INT 5
#define CSF_LF_6_INT 6
#define CSF_LF_7_INT 7
#define CSF_LF_8_INT 8
#define CSF_LF_9_INT 9
#define CSF_LF_0_INT_MASK 0
#define CSF_LF_1_INT_MASK 1
#define CSF_LF_2_INT_MASK 2
#define CSF_LF_3_INT_MASK 3
#define CSF_LF_4_INT_MASK 4
#define CSF_LF_5_INT_MASK 5
#define CSF_LF_6_INT_MASK 6
#define CSF_LF_7_INT_MASK 7
#define CSF_LF_8_INT_MASK 8
#define CSF_LF_9_INT_MASK 9
#define CSF_LF_10_INT 0
#define CSF_LF_11_INT 1
#define CSF_LF_12_INT 2
#define CSF_LF_13_INT 3
#define CSF_LF_14_INT 4
#define CSF_LF_15_INT 5
#define CSF_LF_16_INT 6
#define CSF_LF_17_INT 7
#define CSF_LF_18_INT 8
#define CSF_LF_19_INT 9
#define CSF_LF_10_INT_MASK 0
#define CSF_LF_11_INT_MASK 1
#define CSF_LF_12_INT_MASK 2
#define CSF_LF_13_INT_MASK 3
#define CSF_LF_14_INT_MASK 4
#define CSF_LF_15_INT_MASK 5
#define CSF_LF_16_INT_MASK 6
#define CSF_LF_17_INT_MASK 7
#define CSF_LF_18_INT_MASK 8
#define CSF_LF_19_INT_MASK 9
#define CSF_LF_20_INT 0
#define CSF_LF_21_INT 1
#define CSF_LF_22_INT 2
#define CSF_LF_23_INT 3
#define CSF_LF_24_INT 4
#define CSF_LF_25_INT 5
#define CSF_LF_26_INT 6
#define CSF_LF_27_INT 7
#define CSF_LF_28_INT 8
#define CSF_LF_29_INT 9
#define CSF_LF_20_INT_MASK 0
#define CSF_LF_21_INT_MASK 1
#define CSF_LF_22_INT_MASK 2
#define CSF_LF_23_INT_MASK 3
#define CSF_LF_24_INT_MASK 4
#define CSF_LF_25_INT_MASK 5
#define CSF_LF_26_INT_MASK 6
#define CSF_LF_27_INT_MASK 7
#define CSF_LF_28_INT_MASK 8
#define CSF_LF_29_INT_MASK 9
#define CSF_LF_30_INT 0
#define CSF_LF_31_INT 1
#define CSF_LF_32_INT 2
#define CSF_LF_33_INT 3
#define CSF_LF_34_INT 4
#define CSF_LF_35_INT 5
#define CSF_LF_36_INT 6
#define CSF_LF_37_INT 7
#define CSF_LF_38_INT 8
#define CSF_LF_39_INT 9
#define CSF_LF_30_INT_MASK 0
#define CSF_LF_31_INT_MASK 1
#define CSF_LF_32_INT_MASK 2
#define CSF_LF_33_INT_MASK 3
#define CSF_LF_34_INT_MASK 4
#define CSF_LF_35_INT_MASK 5
#define CSF_LF_36_INT_MASK 6
#define CSF_LF_37_INT_MASK 7
#define CSF_LF_38_INT_MASK 8
#define CSF_LF_39_INT_MASK 9
#define CSF_LF_40_INT 0
#define CSF_LF_41_INT 1
#define CSF_LF_42_INT 2
#define CSF_LF_43_INT 3
#define CSF_LF_44_INT 4
#define CSF_LF_45_INT 5
#define CSF_LF_46_INT 6
#define CSF_LF_47_INT 7
#define CSF_LF_48_INT 8
#define CSF_LF_49_INT 9
#define CSF_LF_40_INT_MASK 0
#define CSF_LF_41_INT_MASK 1
#define CSF_LF_42_INT_MASK 2
#define CSF_LF_43_INT_MASK 3
#define CSF_LF_44_INT_MASK 4
#define CSF_LF_45_INT_MASK 5
#define CSF_LF_46_INT_MASK 6
#define CSF_LF_47_INT_MASK 7
#define CSF_LF_48_INT_MASK 8
#define CSF_LF_49_INT_MASK 9
#define CSF_LF_50_INT 0
#define CSF_LF_51_INT 1
#define CSF_LF_52_INT 2
#define CSF_LF_53_INT 3
#define CSF_LF_54_INT 4
#define CSF_LF_55_INT 5
#define CSF_LF_56_INT 6
#define CSF_LF_57_INT 7
#define CSF_LF_58_INT 8
#define CSF_LF_59_INT 9
#define CSF_LF_50_INT_MASK 0
#define CSF_LF_51_INT_MASK 1
#define CSF_LF_52_INT_MASK 2
#define CSF_LF_53_INT_MASK 3
#define CSF_LF_54_INT_MASK 4
#define CSF_LF_55_INT_MASK 5
#define CSF_LF_56_INT_MASK 6
#define CSF_LF_57_INT_MASK 7
#define CSF_LF_58_INT_MASK 8
#define CSF_LF_59_INT_MASK 9
#define CSF_LF_60_INT 0
#define CSF_LF_61_INT 1
#define CSF_LF_62_INT 2
#define CSF_LF_63_INT 3
#define CSF_LF_64_INT 4
#define CSF_LF_65_INT 5
#define CSF_LF_66_INT 6
#define CSF_LF_67_INT 7
#define CSF_LF_68_INT 8
#define CSF_LF_69_INT 9
#define CSF_LF_60_INT_MASK 0
#define CSF_LF_61_INT_MASK 1
#define CSF_LF_62_INT_MASK 2
#define CSF_LF_63_INT_MASK 3
#define CSF_LF_64_INT_MASK 4
#define CSF_LF_65_INT_MASK 5
#define CSF_LF_66_INT_MASK 6
#define CSF_LF_67_INT_MASK 7
#define CSF_LF_68_INT_MASK 8
#define CSF_LF_69_INT_MASK 9
#define CSF_LF_70_INT 0
#define CSF_LF_71_INT 1
#define CSF_LF_72_INT 2
#define CSF_LF_73_INT 3
#define CSF_LF_74_INT 4
#define CSF_LF_75_INT 5
#define CSF_LF_76_INT 6
#define CSF_LF_77_INT 7
#define CSF_LF_78_INT 8
#define CSF_LF_79_INT 9
#define CSF_LF_70_INT_MASK 0
#define CSF_LF_71_INT_MASK 1
#define CSF_LF_72_INT_MASK 2
#define CSF_LF_73_INT_MASK 3
#define CSF_LF_74_INT_MASK 4
#define CSF_LF_75_INT_MASK 5
#define CSF_LF_76_INT_MASK 6
#define CSF_LF_77_INT_MASK 7
#define CSF_LF_78_INT_MASK 8
#define CSF_LF_79_INT_MASK 9
#define CSF_RF_0_INT 0
#define CSF_RF_1_INT 1
#define CSF_RF_2_INT 2
#define CSF_RF_3_INT 3
#define CSF_RF_4_INT 4
#define CSF_RF_5_INT 5
#define CSF_RF_6_INT 6
#define CSF_RF_7_INT 7
#define CSF_RF_8_INT 8
#define CSF_RF_9_INT 9
#define CSF_RF_0_INT_MASK 0
#define CSF_RF_1_INT_MASK 1
#define CSF_RF_2_INT_MASK 2
#define CSF_RF_3_INT_MASK 3
#define CSF_RF_4_INT_MASK 4
#define CSF_RF_5_INT_MASK 5
#define CSF_RF_6_INT_MASK 6
#define CSF_RF_7_INT_MASK 7
#define CSF_RF_8_INT_MASK 8
#define CSF_RF_9_INT_MASK 9
#define CSF_RF_10_INT 0
#define CSF_RF_11_INT 1
#define CSF_RF_12_INT 2
#define CSF_RF_13_INT 3
#define CSF_RF_14_INT 4
#define CSF_RF_15_INT 5
#define CSF_RF_16_INT 6
#define CSF_RF_17_INT 7
#define CSF_RF_18_INT 8
#define CSF_RF_19_INT 9
#define CSF_RF_10_INT_MASK 0
#define CSF_RF_11_INT_MASK 1
#define CSF_RF_12_INT_MASK 2
#define CSF_RF_13_INT_MASK 3
#define CSF_RF_14_INT_MASK 4
#define CSF_RF_15_INT_MASK 5
#define CSF_RF_16_INT_MASK 6
#define CSF_RF_17_INT_MASK 7
#define CSF_RF_18_INT_MASK 8
#define CSF_RF_19_INT_MASK 9
#define CSF_RF_20_INT 0
#define CSF_RF_21_INT 1
#define CSF_RF_22_INT 2
#define CSF_RF_23_INT 3
#define CSF_RF_24_INT 4
#define CSF_RF_25_INT 5
#define CSF_RF_26_INT 6
#define CSF_RF_27_INT 7
#define CSF_RF_28_INT 8
#define CSF_RF_29_INT 9
#define CSF_RF_20_INT_MASK 0
#define CSF_RF_21_INT_MASK 1
#define CSF_RF_22_INT_MASK 2
#define CSF_RF_23_INT_MASK 3
#define CSF_RF_24_INT_MASK 4
#define CSF_RF_25_INT_MASK 5
#define CSF_RF_26_INT_MASK 6
#define CSF_RF_27_INT_MASK 7
#define CSF_RF_28_INT_MASK 8
#define CSF_RF_29_INT_MASK 9
#define CSF_RF_30_INT 0
#define CSF_RF_31_INT 1
#define CSF_RF_32_INT 2
#define CSF_RF_33_INT 3
#define CSF_RF_34_INT 4
#define CSF_RF_35_INT 5
#define CSF_RF_36_INT 6
#define CSF_RF_37_INT 7
#define CSF_RF_38_INT 8
#define CSF_RF_39_INT 9
#define CSF_RF_30_INT_MASK 0
#define CSF_RF_31_INT_MASK 1
#define CSF_RF_32_INT_MASK 2
#define CSF_RF_33_INT_MASK 3
#define CSF_RF_34_INT_MASK 4
#define CSF_RF_35_INT_MASK 5
#define CSF_RF_36_INT_MASK 6
#define CSF_RF_37_INT_MASK 7
#define CSF_RF_38_INT_MASK 8
#define CSF_RF_39_INT_MASK 9
#define CSF_RF_40_INT 0
#define CSF_RF_41_INT 1
#define CSF_RF_42_INT 2
#define CSF_RF_43_INT 3
#define CSF_RF_44_INT 4
#define CSF_RF_45_INT 5
#define CSF_RF_46_INT 6
#define CSF_RF_47_INT 7
#define CSF_RF_48_INT 8
#define CSF_RF_49_INT 9
#define CSF_RF_40_INT_MASK 0
#define CSF_RF_41_INT_MASK 1
#define CSF_RF_42_INT_MASK 2
#define CSF_RF_43_INT_MASK 3
#define CSF_RF_44_INT_MASK 4
#define CSF_RF_45_INT_MASK 5
#define CSF_RF_46_INT_MASK 6
#define CSF_RF_47_INT_MASK 7
#define CSF_RF_48_INT_MASK 8
#define CSF_RF_49_INT_MASK 9
#define CSF_RF_50_INT 0
#define CSF_RF_51_INT 1
#define CSF_RF_52_INT 2
#define CSF_RF_53_INT 3
#define CSF_RF_54_INT 4
#define CSF_RF_55_INT 5
#define CSF_RF_56_INT 6
#define CSF_RF_57_INT 7
#define CSF_RF_58_INT 8
#define CSF_RF_59_INT 9
#define CSF_RF_50_INT_MASK 0
#define CSF_RF_51_INT_MASK 1
#define CSF_RF_52_INT_MASK 2
#define CSF_RF_53_INT_MASK 3
#define CSF_RF_54_INT_MASK 4
#define CSF_RF_55_INT_MASK 5
#define CSF_RF_56_INT_MASK 6
#define CSF_RF_57_INT_MASK 7
#define CSF_RF_58_INT_MASK 8
#define CSF_RF_59_INT_MASK 9
#define CSF_RF_60_INT 0
#define CSF_RF_61_INT 1
#define CSF_RF_62_INT 2
#define CSF_RF_63_INT 3
#define CSF_RF_64_INT 4
#define CSF_RF_65_INT 5
#define CSF_RF_66_INT 6
#define CSF_RF_67_INT 7
#define CSF_RF_68_INT 8
#define CSF_RF_69_INT 9
#define CSF_RF_60_INT_MASK 0
#define CSF_RF_61_INT_MASK 1
#define CSF_RF_62_INT_MASK 2
#define CSF_RF_63_INT_MASK 3
#define CSF_RF_64_INT_MASK 4
#define CSF_RF_65_INT_MASK 5
#define CSF_RF_66_INT_MASK 6
#define CSF_RF_67_INT_MASK 7
#define CSF_RF_68_INT_MASK 8
#define CSF_RF_69_INT_MASK 9
#define CSF_RF_70_INT 0
#define CSF_RF_71_INT 1
#define CSF_RF_72_INT 2
#define CSF_RF_73_INT 3
#define CSF_RF_74_INT 4
#define CSF_RF_75_INT 5
#define CSF_RF_76_INT 6
#define CSF_RF_77_INT 7
#define CSF_RF_78_INT 8
#define CSF_RF_79_INT 9
#define CSF_RF_70_INT_MASK 0
#define CSF_RF_71_INT_MASK 1
#define CSF_RF_72_INT_MASK 2
#define CSF_RF_73_INT_MASK 3
#define CSF_RF_74_INT_MASK 4
#define CSF_RF_75_INT_MASK 5
#define CSF_RF_76_INT_MASK 6
#define CSF_RF_77_INT_MASK 7
#define CSF_RF_78_INT_MASK 8
#define CSF_RF_79_INT_MASK 9


/******************************************************************************
*
* MOUDLE_NAME
*
*	 mcmaxrx
*
* MOUDEL_ADDRESS
*
*	 mcmaxrx
*
* MOUDLE_OFFSET
*
*    mcmaxrx
*
* MOUDLE_RegisterName
*
*	 mcmaxrx
*
*
******************************************************************************/
/****************mcmaxrx****************/
#define WATCHDOG_EN 0
#define WATCHDOG_TIME 1
#define MACRX_CHAN_EN_0 0
#define MACRX_CHAN_EN_1 1
#define MACRX_CHAN_EN_2 2
#define MACRX_STAT_EN_0 0
#define MACRX_STAT_EN_1 1
#define MACRX_STAT_EN_2 2
#define CRC_CUTOFF_EN_0 0
#define CRC_CUTOFF_EN_1 1
#define CRC_CUTOFF_EN_2 2
#define STA_CLR_CHAN 0
#define STA_CLR_PLS 0
#define ARM_LF_0 0
#define ARM_LF_1 1
#define ARM_LF_2 2
#define ARM_RF_0 3
#define ARM_RF_1 4
#define ARM_RF_2 5
#define ARM_DECODE_ERR_0 6
#define ARM_DECODE_ERR_1 7
#define ARM_DECODE_ERR_2 8
#define ENCODE_ERR_CODE_ECC_ERR_CORRECT1 0
#define DECODE_ERR_CODE_ECC_ERR_CORRECT 1
#define TOTAL_FRAME_ECC_ERR_CORRECT 2
#define GOOD_FRAME_ECC_ERR_CORRECT 3
#define BAD_FRAME_ECC_ERR_CORRECT 4
#define FRAGMENT_FRAME_ECC_ERR_CORRECT 5
#define JABBER_FRAME_ECC_ERR_CORRECT 6
#define OVERSIZE_FRAME_ECC_ERR_CORRECT 7
#define UNICAST_FRAME_ECC_ERR_CORRECT 8
#define MULTICAST_FRAME_ECC_ERR_CORRECT 9
#define BROADCAST_FRAME_ECC_ERR_CORRECT 10
#define B64_FRAME_ECC_ERR_CORRECT 11
#define B65_MPL_FRAME_ECC_ERR_CORRECT 12
#define TOTAL_BYTES_ECC_ERR_CORRECT 13
#define GOOD_BYTES_ECC_ERR_CORRECT 14
#define BAD_BYTES_ECC_ERR_CORRECT 15
#define FRAGMENT_BYTES_ECC_ERR_CORRECT 16
#define JABBER_BYTES_ECC_ERR_CORRECT 17
#define OVERSIZE_BYTES_ECC_ERR_CORRECT 18
#define UNICAST_BYTES_ECC_ERR_CORRECT 19
#define MULTICAST_BYTES_ECC_ERR_CORRECT 20
#define BROADCAST_BYTES_ECC_ERR_CORRECT 21
#define B64_BYTES_ECC_ERR_CORRECT 22
#define B65_MPL_BYTES_ECC_ERR_CORRECT 23
#define ENCODE_ERR_CODE_ECC_ERR_UNCORRECT1 0
#define DECODE_ERR_CODE_ECC_ERR_UNCORRECT 1
#define TOTAL_FRAME_ECC_ERR_UNCORRECT 2
#define GOOD_FRAME_ECC_ERR_UNCORRECT 3
#define BAD_FRAME_ECC_ERR_UNCORRECT 4
#define FRAGMENT_FRAME_ECC_ERR_UNCORRECT 5
#define JABBER_FRAME_ECC_ERR_UNCORRECT 6
#define OVERSIZE_FRAME_ECC_ERR_UNCORRECT 7
#define UNICAST_FRAME_ECC_ERR_UNCORRECT 8
#define MULTICAST_FRAME_ECC_ERR_UNCORRECT 9
#define BROADCAST_FRAME_ECC_ERR_UNCORRECT 10
#define B64_FRAME_ECC_ERR_UNCORRECT 11
#define B65_MPL_FRAME_ECC_ERR_UNCORRECT 12
#define TOTAL_BYTES_ECC_ERR_UNCORRECT 13
#define GOOD_BYTES_ECC_ERR_UNCORRECT 14
#define BAD_BYTES_ECC_ERR_UNCORRECT 15
#define FRAGMENT_BYTES_ECC_ERR_UNCORRECT 16
#define JABBER_BYTES_ECC_ERR_UNCORRECT 17
#define OVERSIZE_BYTES_ECC_ERR_UNCORRECT 18
#define UNICAST_BYTES_ECC_ERR_UNCORRECT 19
#define MULTICAST_BYTES_ECC_ERR_UNCORRECT 20
#define BROADCAST_BYTES_ECC_ERR_UNCORRECT 21
#define B64_BYTES_ECC_ERR_UNCORRECT 22
#define B65_MPL_BYTES_ECC_ERR_UNCORRECT 23
#define EOP_ABNOR_ALM_0 0
#define EOP_ABNOR_ALM_1 1
#define EOP_ABNOR_ALM_2 2
#define SOP_WARNING 0
#define EOP_WARNING 1
#define SW_RAM_FULL 2
#define SOP_CNT 0
#define EOP_CNT 0

/******************************************************************************
*
* MOUDLE_NAME
*
*	 mcmaxtx
*
* MOUDEL_ADDRESS
*
*	 mcmaxtx
*
* MOUDLE_OFFSET
*
*    mcmaxtx
*
* MOUDLE_RegisterName
*
*	 mcmaxtx
*
*
******************************************************************************/
/****************mcmaxtx****************/
#define MAC_SAR_MODE_0 0
#define MAC_SAR_MODE_1 1
#define MAC_SAR_MODE_2 2
#define LOCAL_FAULT_CONFIG 0
#define REMOTE_FAULT_CONFIG 1
#define TX_TEST_EN 2
#define MANUAL_LF_INSERT 3
#define MANUAL_IDLE_INSERT 4
#define ENABLE_TX_FAULT 5
#define MANUAL_RF_INSERT 6
#define STAT_EN_0 0
#define STAT_EN_1 1
#define STAT_EN_2 2
#define AVG_IPG 0
#define MINIMUM_MODE 1
#define CRC_INC_EN_0 0
#define CRC_INC_EN_1 1
#define CRC_INC_EN_2 2
#define CRC_RECAL_EN_0 0
#define CRC_RECAL_EN_1 1
#define CRC_RECAL_EN_2 2
#define STA_ALL_CLR 0
#define RX_STA_RAM_INIT_DONE 0
#define CRC_ERRDROP_EN_0 0
#define CRC_ERRDROP_EN_1 1
#define CRC_ERRDROP_EN_2 2
#define DECODE_SEL 0
#define STA_CLR 0
#define TS_CONFIG_DONE 0
#define TS_RAM_INIT 0
#define ENV_RAM_INIT 0
#define ENV_RAM_INIT_DONE 0
#define TS_RAM_INIT_DONE 1
#define STA_RAM_INIT_DONE 2
#define UNDERFLOW_0 0
#define UNDERFLOW_1 1
#define UNDERFLOW_2 2
#define SAR_LF_ALM_0 0
#define SAR_LF_ALM_1 1
#define SAR_LF_ALM_2 2
#define SAR_RF_ALM_0 0
#define SAR_RF_ALM_1 1
#define SAR_RF_ALM_2 2
#define ENCODE_ERR_ALM_0 0
#define ENCODE_ERR_ALM_1 1
#define ENCODE_ERR_ALM_2 2
#define DECODE_ERR_ALM_0 0
#define DECODE_ERR_ALM_1 1
#define DECODE_ERR_ALM_2 2
#define MACTX_ENCODE_ERR_CODE_ECC_ERR_CORRECT 0
#define MACTX_DECODE_ERR_CODE_ECC_ERR_CORRECT 1
#define MACTX_TOTAL_FRAME_ECC_ERR_CORRECT 2
#define MACTX_GOOD_FRAME_ECC_ERR_CORRECT 3
#define MACTX_BAD_FRAME_ECC_ERR_CORRECT 4
#define MACTX_SAR_FRAGMENT_FRAME_ECC_ERR_CORRECT 5
#define MACTX_SAR_JABBER_FRAME_ECC_ERR_CORRECT 6
#define MACTX_SAR_OVERSIZE_FRAME_ECC_ERR_CORRECT 7
#define MACTX_UNICAST_FRAME_ECC_ERR_CORRECT 8
#define MACTX_MULTICAST_FRAME_ECC_ERR_CORRECT 9
#define MACTX_BROADCAST_FRAME_ECC_ERR_CORRECT 10
#define MACTX_B64_FRAME_ECC_ERR_CORRECT 11
#define MACTX_B65_MPL_FRAME_ECC_ERR_CORRECT 12
#define MACTX_TOTAL_BYTES_ECC_ERR_CORRECT 13
#define MACTX_GOOD_BYTES_ECC_ERR_CORRECT 14
#define MACTX_BAD_BYTES_ECC_ERR_CORRECT 15
#define MACTX_SAR_FRAGMENT_BYTES_ECC_ERR_CORRECT 16
#define MACTX_SAR_JABBER_BYTES_ECC_ERR_CORRECT 17
#define MACTX_SAR_OVERSIZE_BYTES_ECC_ERR_CORRECT 18
#define MACTX_UNICAST_BYTES_ECC_ERR_CORRECT 19
#define MACTX_MULTICAST_BYTES_ECC_ERR_CORRECT 20
#define MACTX_BROADCAST_BYTES_ECC_ERR_CORRECT 21
#define MACTX_B64_BYTES_ECC_ERR_CORRECT 22
#define MACTX_B65_MPL_BYTES_ECC_ERR_CORRECT 23
#define MACTX_ENCODE_ERR_CODE_ECC_ERR_UNCORRECT 0
#define MACTX_DECODE_ERR_CODE_ECC_ERR_UNCORRECT 1
#define MACTX_TOTAL_FRAME_ECC_ERR_UNCORRECT 2
#define MACTX_GOOD_FRAME_ECC_ERR_UNCORRECT 3
#define MACTX_BAD_FRAME_ECC_ERR_UNCORRECT 4
#define MACTX_SAR_FRAGMENT_FRAME_ECC_ERR_UNCORRECT 5
#define MACTX_SAR_JABBER_FRAME_ECC_ERR_UNCORRECT 6
#define MACTX_SAR_OVERSIZE_FRAME_ECC_ERR_UNCORRECT 7
#define MACTX_UNICAST_FRAME_ECC_ERR_UNCORRECT 8
#define MACTX_MULTICAST_FRAME_ECC_ERR_UNCORRECT 9
#define MACTX_BROADCAST_FRAME_ECC_ERR_UNCORRECT 10
#define MACTX_B64_FRAME_ECC_ERR_UNCORRECT 11
#define MACTX_B65_MPL_FRAME_ECC_ERR_UNCORRECT 12
#define MACTX_TOTAL_BYTES_ECC_ERR_UNCORRECT 13
#define MACTX_GOOD_BYTES_ECC_ERR_UNCORRECT 14
#define MACTX_BAD_BYTES_ECC_ERR_UNCORRECT 15
#define MACTX_SAR_FRAGMENT_BYTES_ECC_ERR_UNCORRECT 16
#define MACTX_SAR_JABBER_BYTES_ECC_ERR_UNCORRECT 17
#define MACTX_SAR_OVERSIZE_BYTES_ECC_ERR_UNCORRECT 18
#define MACTX_UNICAST_BYTES_ECC_ERR_UNCORRECT 19
#define MACTX_MULTICAST_BYTES_ECC_ERR_UNCORRECT 20
#define MACTX_BROADCAST_BYTES_ECC_ERR_UNCORRECT 21
#define MACTX_B64_BYTES_ECC_ERR_UNCORRECT 22
#define MACTX_B65_MPL_BYTES_ECC_ERR_UNCORRECT 23


/******************************************************************************
*
* MOUDLE_NAME
*
*	 ohoam_ef
*
* MOUDEL_ADDRESS
*
*	 ohoam_ef
*
* MOUDLE_OFFSET
*
*    ohoam_ef
*
* MOUDLE_RegisterName
*
*	 ohoam_ef
*
*
******************************************************************************/
/***********ohoam_ef****************/
#define SOH_INS_DRAM_INIT_EN 0
#define SOH_INST_MODE 1
#define SOH_INS_CHKSH_MFI 2
#define SOH_INS_DBUFF_LVL_CLR 3
#define SOH_INS_DBUFF_LVL_RPT_SEL 4
#define SOAM_NO_OVERTIME_PRO 5
#define SOAM_OVERTIME 6
#define SOAM_FLEXE_LOOPBACK 0
#define SOAM_MAC_LOOPBACK 1
#define MAC_INTE_RAM_INIT_EN 2
#define FLEXE_INTE_RAM_INIT_EN 3
#define SOAM_FLEXE_CFG_DONE 4
#define SOAM_MAC_CFG_DONE 5
#define ASIC_SOAM_FLEXE_ALM_EXT_EN 6
#define ASIC_SOAM_MAC_ALM_EXT_EN 7
#define SOH_LOOPBACK 8
#define SOAM_FLEXE_BAS_EXT_EN 9
#define SOAM_MAC_BAS_EXT_EN 10
#define SOAM_FLEXE_FLVL_CLR 11
#define SOAM_MAC_FLVL_CLR 12
#define SOAM_FLEXE_ALM_PRD 0
#define SOAM_MAC_ALM_PRD 1
#define SOAM_TIMER_STEP_CFG 2
#define ASIC_OHIF_LOCAL_ADDR_0 0
#define ASIC_OHIF_LOCAL_ADDR_1 1
#define ASIC_OHIF_DEST_ADDR_0 2
#define ASIC_OHIF_DEST_ADDR_1 3
#define SOAM_FLEXE_DSCFLVL_STAT 0
#define SOAM_MAC_DSCFLVL_STAT 1
#define SOH_INS_DBUFF_MAX_LVL 2
#define SOH_INS_PIDERR_ALARM 0
#define SOH_INS_REQ_FIFO_FULL_ALAM 1
#define SOH_INS_DA_ERR 2
#define SOHSH_ERR 3
#define INS_MFI_CON_ERR 4
#define INS_CBAS_PLN_ERR 5
#define INS_ETHID_ERR 6
#define SOH_EXT_CFFULL_ALARM 0
#define SOH_INS_DBUFF_EMPTY 1
#define SOH_INS_DBUFF_FULL 2
#define FLEXE_EXT_CHID_ERR_ALAM 0
#define FLEXE_EXT_SEQ_ILGL_ALAM 1
#define FLEXE_EXT_SOP_ONLY_ALAM 2
#define FLEXE_EXT_EOP_ONLY_ALAM 3
#define FLEXE_EXT_DSC_FIFO_FULL 4
#define FLEXE_SOAM_OVERTIME_ALM 5
#define MAC_EXT_CHID_ERR_ALAM 0
#define MAC_EXT_SEQ_ILGL_ALAM 1
#define MAC_EXT_SOP_ONLY_ALAM 2
#define MAC_EXT_EOP_ONLY_ALAM 3
#define MAC_EXT_DSC_FIFO_FULL 4
#define MAC_SOAM_OVERTIME_ALM 5
#define EXT_GBF_BACKPRE_LVL 0
#define EXT_GBF_IDLEADD_LVL 1
#define B66ENCD_TESTEN 2
#define B66ENCD_BKFTEN 3
#define B66ENCD_RFCFG 4
#define B66ENCD_LFCFG 5
#define B66ENCD_MANRF 6
#define B66ENCD_MANLF 7
#define B66ENCD_MANIDL 8
#define B66DECODE_SEL 9
#define EXT_GBF_FULL_ALM 0
#define EXT_GBF_EMPTY_ALM 1
#define B66_ENCODE_ERR_ALM 2
#define INS_GBF_FULL_ALM 3
#define INS_PREF_FULL_ALM 4
#define FLEXE_SOAM_REQF_FULL_ALM 5
#define MAC_SOAM_REQF_FULL_ALM 6
#define B66_DECODE_ERR_ALM 7
#define B66_DECODE_EOP_ABNOR_ALM 8
#define DE2EN_RF_STATUS 9
#define DE2EN_LF_STATUS 10
#define ENCODE_ERR_CNT 0
#define DECODE_ERR_CNT 1
#define B66_INS_BYTES_CNT 2
#define B66_DECODE_PKT_CNT 0
#define EXT_REQ_PKT_CNT 1
#define INS_PKT_CRC_ERR_CNT 0
#define INS_SHORT_PKT_CNT 1
#define EXT_PKT_SOP_CNT 0
#define EXT_OAM_REQ_CNT 1
#define OAM_SEQ_ERR_CHID 0
#define OAM_SEQ_ERR_EN 1
#define OAM_INS_MODE 2
#define SELF_MAX_CHID 3
#define SET_CRC_ERR 4
#define TEST_OH_EXT_ERR_0 0
#define TEST_OH_EXT_ERR_1 1
#define TEST_TOLIUF_FULL_ALM 0
#define EXT_ETH_DNT_SQT 1
#define EXT_OAMREQ_DNT_SQT 2
#define REQ_MFI_CON_ERR 3
#define EXT_MFI_CON_ERR 4
#define EXT_SSF_ALM 5
#define EXT_COL_GEN_M 0
#define EXT_COL_GEN_BASE_M 0
#define SOAM_FLEXE_SELF_PLS 0
#define SOAM_MAC_SELF_PLS 1
#define FLEXE_OAM_CMP_ALM_0 0
#define FLEXE_OAM_CMP_ALM_1 1
#define FLEXE_OAM_CMP_ALM_2 2
#define MAC_OAM_CMP_ALM_0 0
#define MAC_OAM_CMP_ALM_1 1
#define MAC_OAM_CMP_ALM_2 2

/******************************************************************************
*
* MOUDLE_NAME
*
*	 b66switch
*
* MOUDEL_ADDRESS
*
*	 b66switch
*
* MOUDLE_OFFSET
*
*    b66switch
*
* MOUDLE_RegisterName
*
*	 b66switch
*
*
******************************************************************************/
/************b66switch***********/
#define LEVELCLR_SCH 0
#define MAXLEVEL_SCH_0 0
#define MAXLEVEL_SCH_1 1
#define MAXLEVEL_SCH_2 2
#define MAXLEVEL_SCH_3 3
#define MAXLEVEL_SCH_4 4
#define FULL_SCH_0 0
#define FULL_SCH_1 1
#define FULL_SCH_2 2
#define FULL_SCH_3 3
#define FULL_SCH_4 4
#define PROTECT_EN 0
#define CHID_CFG 0
#define PROTECT_INFO_0 0
#define PROTECT_INFO_1 1
#define PROTECT_INFO_2 2
#define PROTECT_INFO_3 3
#define PROTECT_INFO_4 4
#define PROTECT_INFO_5 5
#define PROTECT_INFO_6 6
#define PROTECT_INFO_7 7
#define PROTECT_INFO_8 8
#define PROTECT_INFO_9 9
#define PROTECT_INFO_10 10
#define PROTECT_INFO_11 11
#define PROTECT_INFO_12 12
#define PROTECT_INFO_13 13
#define PROTECT_INFO_14 14
#define PROTECT_INFO_15 15

/******************************************************************************
*
* MOUDLE_NAME
*
*	 channelize
*
* MOUDEL_ADDRESS
*
*	 channelize
*
* MOUDLE_OFFSET
*
*    channelize
*
* MOUDLE_RegisterName
*
*	 channelize
*
*
******************************************************************************/
/************channelize************/
#define LEVELCLR_CHANNELIZE 0
#define BYPASS_EN 0
#define B66REPLACE_EN 1
#define B66REPLACE_PATTERN 2
#define PCSCHID_CFG_0 0
#define PCSCHID_CFG_1 1
#define MAXLEVEL_CHANNELIZE_0 0
#define MAXLEVEL_CHANNELIZE_1 1
#define FULL_CHANNELIZE 0

/******************************************************************************
*
* MOUDLE_NAME
*
*	 serdes
*
* MOUDEL_ADDRESS
*
*	 serdes
*
* MOUDLE_OFFSET
*
*    serdes
*
* MOUDLE_RegisterName
*
*	 serdes
*
*
******************************************************************************/
/***********serdes**************/
#define SERDES_BRG_RDATA 0
#define SERDES_BRG_WDATA 0
#define SERDES_BRG_ADDR 0
#define SERDES_BRG_WR 1
#define SERDES_BRG_RD_ACK 2
#define LINE100GE_1_LOCK2DATA 0
#define LINE100GE_1_LOCK2REF 1
#define LINE100GE_2_LOCK2DATA 2
#define LINE100GE_2_LOCK2REF 3
#define LINE50GE_1_LOCK2DATA 4
#define LINE50GE_1_LOCK2REF 5
#define LINE50GE_2_LOCK2DATA 6
#define LINE50GE_2_LOCK2REF 7
#define LINE50GE_3_LOCK2DATA 8
#define LINE50GE_3_LOCK2REF 9
#define LINE50GE_4_LOCK2DATA 10
#define LINE50GE_4_LOCK2REF 11
#define BP_ILK_300G_LOCK2DATA 12
#define BP_ILK_300G_LOCK2REF 13
#define BP_ILK_150G_LOCK2DATA 14
#define BP_ILK_150G_LOCK2REF 15
#define GE_1_LOCK2DATA 16
#define GE_1_LOCK2REF 17
#define GE_2_LOCK2DATA 18
#define GE_2_LOCK2REF 19
#define GE_3_LOCK2DATA 20
#define GE_3_LOCK2REF 21
#define FPGA_INFO 0
#define FPGA_FUNCTION_INFO 0

/******************************************************************************
*
* MOUDLE_NAME
*
*	 ilkn_wrap
*
* MOUDEL_ADDRESS
*
*	 ilkn_wrap
*
* MOUDLE_OFFSET
*
*    ilkn_wrap
*
* MOUDLE_RegisterName
*
*	 ilkn_wrap
*
*
******************************************************************************/
/************ilkn_wrap***************/
#define CFG_TX_EN 0
#define CFG_RX_EN 1
#define CFG_SYS_RX_LPBK_LOCAL_EN 2
#define CFG_SYS_AFTER_REORDER_RX_LPBK_EN 3
#define CFG_FC_BIT_ORDER_INVERSE 4
#define FRM_MAX_128BYTE 5
#define SYS_TX_FC_CH_CNT_SEL 6
#define CFG_SYS_TX_FIFO_PAF 7
#define SYS_TX_ERR_PACKET_PROTOCOL 0
#define SYS_TX_ERR_FIFO_OVERFLOW 1
#define SYS_RX_ERR_PACKET_PROTOCOL 2
#define SYS_RX_ERR_FIFO_OVERFLOW 3
#define SYS_RX_ERR_BURST_CNT 4
#define SYS_TX_ERR_SEGMENT_PROTOCOL 5
#define SYS_TX_ERR_FIFO_UNDERRUN 6
#define SYS_TX_CB_ERR_BURST 7
#define SYS_TX_CB_ERR_PKT 8
#define SYS_TX_ERR_BURST_PROTOCOL 9
#define PHY_RX_BLOCK_LOCK_W 0
#define PHY_RX_FRAME_LOCK_W 1
#define PCS_TX_ERR_FATAL 2
#define PCS_RX_STATUS 3
#define PHY_PLL_LOCKED 4
#define BURST_SHORT 0
#define BURST_MAX 1
#define BURST_MIN 2
#define PHY_RX_IS_LOCKEDTOREF_W 0
#define PHY_RX_IS_LOCKEDTODATA_2 1
#define CHAN_MAPPING_0_3 0
#define CHAN_MAPPING_4_7 1
#define CHAN_MAPPING_8_11 2
#define CHAN_MAPPING_12_15 3
#define CHAN_MAPPING_16_19 4
#define CHAN_MAPPING_20_23 5
#define CHAN_MAPPING_24_27 6
#define CHAN_MAPPING_28_31 7
#define CHAN_MAPPING_32_35 8
#define CHAN_MAPPING_36_39 9
#define DEMAPPING_FIELD_0 0
#define DEMAPPING_FIELD_1 1
#define DEMAPPING_FIELD_2 2
#define DEMAPPING_FIELD_3 3

/******************************************************************************
*
* MOUDLE_NAME
*
*	 top
*
* MOUDEL_ADDRESS
*
*	 top
*
* MOUDLE_OFFSET
*
*    top
*
* MOUDLE_RegisterName
*
*	 top
*
*
******************************************************************************/
/************top*****************/
#define RST_GLB_FUCI_SOFT_N 0
#define RST_GLB_LOGIC_N 0
#define RST_FLEXE_MUX_N 0
#define RST_OAM_RX_N 1
#define RST_DEMUX_SYS_N 2
#define RST_CHANNELIZE_SYS_N 3
#define RST_B66SWITCH_SYS_N 4
#define RST_SOH_EXT_SCH_N 5
#define RST_SOH_INS_SCH_N 6
#define RST_SOAM_EXT_330M_0_N 7
#define RST_SOAM_EXT_330M_1_N 8
#define RST_SOAM_EXT_SCH_N 9
#define RST_SOAM_INS_DISTR_N 10
#define RST_SYS_DW_N 11
#define RST_SYS_UP_N 12
#define RST_SYS_TS_N 13
#define RST_FEC_ENC_N 14
#define RST_DEC_ENC_N 15
#define RST_SYS_CPBEGRESS_N 16
#define RST_CPB_INGRESS_N 17
#define RST_ENCODE_N 18
#define RST_DECODE_N 19
#define RST_RATEADP_N 20
#define RST_OAM_TX_N 21
#define RST_SYS_MCMACTX_N 22
#define RST_SYS_MCMACRX_N 23
#define RST_PPSTOD_N 24
#define RST_PCS_TX_ENV_N 25
#define RST_PCS_RX_SMOOTH_N 26
#define RST_FLEXE_MAC_N 27
#define RST_ILK_N 28
#define RST_SERDES_100GE1_N 0
#define RST_SERDES_100GE2_N 1
#define RST_SERDES_50GE1_N 2
#define RST_SERDES_50GE2_N 3
#define RST_SERDES_50GE3_N 4
#define RST_SERDES_50GE4_N 5
#define RST_SERDES_GE_N 6
#define RST_SERDES_GE1_N 7
#define RST_SERDES_GE2_N 8
#define RST_SERDES_ILK_N 9
#define RST_SERDES_CFG_UPI_N 10
#define RST_OH_RX_0_N 0
#define RST_OH_RX_1_N 1
#define RST_OH_RX_2_N 2
#define RST_OH_RX_3_N 3
#define RST_OH_RX_4_N 4
#define RST_OH_RX_5_N 5
#define RST_OH_RX_6_N 6
#define RST_OH_RX_7_N 7
#define RST_OH_TX_0_N 8
#define RST_OH_TX_1_N 9
#define RST_OH_TX_2_N 10
#define RST_OH_TX_3_N 11
#define RST_OH_TX_4_N 12
#define RST_OH_TX_5_N 13
#define RST_OH_TX_6_N 14
#define RST_OH_TX_7_N 15
#define RST_DESKEW_0_N 16
#define RST_DESKEW_1_N 17
#define RST_DESKEW_2_N 18
#define RST_DESKEW_3_N 19
#define RST_DESKEW_4_N 20
#define RST_DESKEW_5_N 21
#define RST_DESKEW_6_N 22
#define RST_DESKEW_7_N 23
#define RST_SYS_HANDLING_N 24
#define RST_SYS_B66SARRX_N 25
#define RST_SYS_OAM_RX_N 26
#define RST_SYS_PORTSCH_DW_N 27
#define RST_SYS_OAM_TX_N 28
#define RST_SYS_RATEADP_N 29
#define RST_SYS_B66SARTX_N 30
#define RST_125M_PPS_N 0
#define RST_UPI_FLEXE_MUX_N 0
#define RST_OAM_RX_UPI_N 1
#define RST_MR_PCS_RX_UPI_N 2
#define RST_DEMUX_UPI_SYS_N 3
#define RST_CHANNELIZE_UPI_SYS_N 4
#define RST_B66SWITCH_UPI_SYS_N 5
#define RST_SOH_UPI_DCI_N 6
#define RST_SYS_DW_UPI_N 7
#define RST_SYS_UP_UPI_N 8
#define RST_FEC_ENC_UPI_N 9
#define RST_FEC_DEC_UPI_N 10
#define RST_CPB_EGRESS_UPI_N 11
#define RST_CPB_INGRESS_UPI_N 12
#define RST_MR_PCS_TX_UPI_N 13
#define RST_RATEADP_UPI_N 14
#define RST_OAM_TX_UPI_N 15
#define RST_MACTX_UPI_N 16
#define RST_MACRX_UPI_N 17
#define RST_DESKEW_FUCI_0_N 18
#define RST_DESKEW_FUCI_1_N 19
#define RST_DESKEW_FUCI_2_N 20
#define RST_DESKEW_FUCI_3_N 21
#define RST_DESKEW_FUCI_4_N 22
#define RST_DESKEW_FUCI_5_N 23
#define RST_DESKEW_FUCI_6_N 24
#define RST_DESKEW_FUCI_7_N 25
#define RST_TX_ENV_GEARBOX_UPI_N 26
#define RST_RX_ENV_GEARBOX_UPI_N 27
#define RST_FLEXE_MAC_UPI_N 28
#define RST_FUCI_ILKN_N 29
#define RST_OH_RX_FUCI_0_N 0
#define RST_OH_RX_FUCI_1_N 1
#define RST_OH_RX_FUCI_2_N 2
#define RST_OH_RX_FUCI_3_N 3
#define RST_OH_RX_FUCI_4_N 4
#define RST_OH_RX_FUCI_5_N 5
#define RST_OH_RX_FUCI_6_N 6
#define RST_OH_RX_FUCI_7_N 7
#define RST_OH_TX_FUCI_0_N 8
#define RST_OH_TX_FUCI_1_N 9
#define RST_OH_TX_FUCI_2_N 10
#define RST_OH_TX_FUCI_3_N 11
#define RST_OH_TX_FUCI_4_N 12
#define RST_OH_TX_FUCI_5_N 13
#define RST_OH_TX_FUCI_6_N 14
#define RST_OH_TX_FUCI_7_N 15
#define RST_SYS_UPI_HANDLING_N 16
#define RST_SYS_UPI_CPBEGRESS_N 17
#define RST_SYS_UPI_MCMACTX_N 18
#define RST_SYS_UPI_B66SARRX_N 19
#define RST_SYS_UPI_OAM_RX_N 20
#define RST_SYS_UPI_OAM_TX_N 21
#define RST_SYS_UPI_RATEADP_N 22
#define RST_SYS_UPI_B66SARTX_N 23
#define RST_SYS_UPI_MCMACRX_N 24
#define RST_INF_CH_ADP_RX_UPI_N 25
#define RST_INF_CH_ADP_TX_UPI_N 26
#define RST_FLEXE_ENV_UPI_N 27
#define RST_REQ_GEN_UPI_N 28
#define PPS_TIME_CFG 0
#define PPS_TOD_RX_NUM 0
#define PPS_TOD_RX_TYPE 0
#define PPS_TOD_RX_LEN 0
#define PPS_TOD_RX_TOD_BYTE0 0
#define PPS_TOD_RX_TOD_SEC_0 0
#define PPS_TOD_RX_TOD_SEC_1 1
#define GLB_CNT_CLR 0
#define GLB_ALM_CLR 1
#define GLB_INT_CLR 2
#define PPS_TOD_RX_TOD_NSEC 0
#define PPS_TOD_RX_TOD_RES 0
#define PPS_TOD_RX_CRC 0
#define PPS_TOD_RX_REAL_TIME_0 0
#define PPS_TOD_RX_REAL_TIME_1 1
#define PPS_TOD_RX_REAL_TIME_2 2
#define PPS_TOD_RX_CNT_TOD_LOST 0
#define PPS_TOD_RX_CNT_PPS_ADJUST 0
#define PPS_TOD_RX_CNT_CRC_EOR 0
#define PPS_TOD_RX_CNT_WAVE_BROKEN 0
#define PPS_TOD_RX_REG_CNT_25M 0
#define RST_SOH_EXT_125M_N 0
#define RST_SOH_INS_125M_N 0
#define RST_SOAM_EXT_125M_N 0
#define RST_SOAM_INS_125M_N 0
#define RST_GEARBOX_RX_N 0
#define RST_GEARBOX_TX_N 1
#define RST_CLK_MEASURE_N 2
#define RST_GEARBOX_RX_REQ_N 3
#define RST_GEARBOX_TX_REQ_N 4
#define PHY_SEL_CFG 0
#define RST_311M_B66SARRX_N 0
#define RST_311M_B66SARTX_N 1
#define RST_311M_TS_N 2
#define RST_311M_UPI_B66SARTX_N 0
#define RST_311M_UPI_B66SARRX_N 1
#define RST_SOH_UPI_125EXT_N 0
#define RST_SOH_UPI_125INS_N 0
#define RST_SOAM_UPI_125EXT_N 0
#define RST_SOAM_UPI_125INS_N 0
#define RST_SOH_UPI_FUCI_N 0
#define RST_SUBSYS_MAC_FUCI_N 1
#define MEASURE_TIME 0
#define MEASURE_CLR 1
#define MEASURE_CNT_MAX 0
#define MEASURE_CNT_MIN 0
#define RST_SYS_PORTSCH_UP_N 0
#define RST_INF_CH_ADP_RX_N 1
#define RST_INF_CH_ADP_TX_N 2
#define RST_FLEXE_ENV_N 3
#define RST_REQ_GEN_N 4
#define RST_MR_PCS_RX_0_N 5
#define RST_MR_PCS_RX_1_N 6
#define RST_MR_PCS_RX_2_N 7
#define RST_MR_PCS_RX_3_N 8
#define RST_MR_PCS_TX_0_N 9
#define RST_MR_PCS_TX_1_N 10
#define RST_MR_PCS_TX_2_N 11
#define RST_MR_PCS_TX_3_N 12
#define RST_FEC_DEC_0_N 13
#define RST_FEC_DEC_1_N 14
#define RST_FEC_ENC_0_N 15
#define RST_FEC_ENC_1_N 16
#define RST_OH_N 17
#define RST_OHIF_1588_RATE_LIMIT_N 18
#define CDR_SEL 0
#define RST_FLEXE_PTP_RX_0_N 0
#define RST_FLEXE_PTP_RX_1_N 1
#define RST_FLEXE_PTP_RX_2_N 2
#define RST_FLEXE_PTP_RX_3_N 3
#define RST_FLEXE_PTP_RX_4_N 4
#define RST_FLEXE_PTP_RX_5_N 5
#define RST_FLEXE_PTP_RX_6_N 6
#define RST_FLEXE_PTP_RX_7_N 7
#define RST_FLEXE_PTP_RX_8_N 8
#define RST_FLEXE_PTP_RX_9_N 9
#define RST_FLEXE_PTP_RX_10_N 10
#define RST_FLEXE_PTP_RX_11_N 11
#define RST_FLEXE_PTP_RX_12_N 12
#define RST_FLEXE_PTP_RX_13_N 13
#define RST_FLEXE_PTP_RX_14_N 14
#define RST_FLEXE_PTP_RX_15_N 15
#define RST_FLEXE_PTP_TX_0_N 16
#define RST_FLEXE_PTP_TX_1_N 17
#define RST_FLEXE_PTP_TX_2_N 18
#define RST_FLEXE_PTP_TX_3_N 19
#define RST_FLEXE_PTP_TX_4_N 20
#define RST_FLEXE_PTP_TX_5_N 21
#define RST_FLEXE_PTP_TX_6_N 22
#define RST_FLEXE_PTP_TX_7_N 23
#define RST_XGMII_RX_N 24
#define RST_XGMII_TX_N 25
#define RST_CPU_RX_N 26
#define RST_CPU_TX_N 27
#define RST_UPI_FLEXE_PTP_RX_0_N 0
#define RST_UPI_FLEXE_PTP_RX_1_N 1
#define RST_UPI_FLEXE_PTP_RX_2_N 2
#define RST_UPI_FLEXE_PTP_RX_3_N 3
#define RST_UPI_FLEXE_PTP_RX_4_N 4
#define RST_UPI_FLEXE_PTP_RX_5_N 5
#define RST_UPI_FLEXE_PTP_RX_6_N 6
#define RST_UPI_FLEXE_PTP_RX_7_N 7
#define RST_UPI_FLEXE_PTP_RX_8_N 8
#define RST_UPI_FLEXE_PTP_RX_9_N 9
#define RST_UPI_FLEXE_PTP_RX_10_N 10
#define RST_UPI_FLEXE_PTP_RX_11_N 11
#define RST_UPI_FLEXE_PTP_RX_12_N 12
#define RST_UPI_FLEXE_PTP_RX_13_N 13
#define RST_UPI_FLEXE_PTP_RX_14_N 14
#define RST_UPI_FLEXE_PTP_RX_15_N 15
#define RST_UPI_FLEXE_PTP_TX_0_N 16
#define RST_UPI_FLEXE_PTP_TX_1_N 17
#define RST_UPI_FLEXE_PTP_TX_2_N 18
#define RST_UPI_FLEXE_PTP_TX_3_N 19
#define RST_UPI_FLEXE_PTP_TX_4_N 20
#define RST_UPI_FLEXE_PTP_TX_5_N 21
#define RST_UPI_FLEXE_PTP_TX_6_N 22
#define RST_UPI_FLEXE_PTP_TX_7_N 23
#define RST_UPI_GLOBAL_IEEE1588_N 24
#define RST_UPI_XGMII_RX_N 25
#define RST_UPI_XGMII_TX_N 26
#define RST_UPI_CCU_GE_N 27
#define RST_UPI_CPU_N 28
#define OH_RX0_INT 0
#define OH_RX1_INT 1
#define OH_RX2_INT 2
#define OH_RX3_INT 3
#define OH_RX4_INT 4
#define OH_RX5_INT 5
#define OH_RX6_INT 6
#define OH_RX7_INT 7
#define FLEXE_OAM_RX0_INT 8
#define FLEXE_OAM_RX2_INT 9
#define FLEXE_OAM_RX3_INT 10
#define FLEXE_OAM_RX4_INT 11
#define FLEXE_OAM_RX5_INT 12
#define FLEXE_OAM_RX6_INT 13
#define FLEXE_OAM_RX7_INT 14
#define FLEXE_OAM_RX8_INT 15
#define FLEXE_OAM_RX9_INT 16
#define FLEXE_OAM_RX10_INT 17
#define FLEXE_OAM_RX11_INT 18
#define FLEXE_OAM_TX0_INT 19
#define FLEXE_OAM_TX1_INT 20
#define FLEXE_OAM_TX2_INT 21
#define SAR_RX_INT 22
#define SAR_TX_INT 23
#define SAR_OAM_RX0_INT 24
#define SAR_OAM_RX2_INT 25
#define SAR_OAM_RX3_INT 26
#define SAR_OAM_RX4_INT 27
#define SAR_OAM_RX5_INT 28
#define SAR_OAM_RX6_INT 29
#define OH_RX0_INT_MASK 0
#define OH_RX1_INT_MASK 1
#define OH_RX2_INT_MASK 2
#define OH_RX3_INT_MASK 3
#define OH_RX4_INT_MASK 4
#define OH_RX5_INT_MASK 5
#define OH_RX6_INT_MASK 6
#define OH_RX7_INT_MASK 7
#define FLEXE_OAM_RX0_INT_MASK 8
#define FLEXE_OAM_RX2_INT_MASK 9
#define FLEXE_OAM_RX3_INT_MASK 10
#define FLEXE_OAM_RX4_INT_MASK 11
#define FLEXE_OAM_RX5_INT_MASK 12
#define FLEXE_OAM_RX6_INT_MASK 13
#define FLEXE_OAM_RX7_INT_MASK 14
#define FLEXE_OAM_RX8_INT_MASK 15
#define FLEXE_OAM_RX9_INT_MASK 16
#define FLEXE_OAM_RX10_INT_MASK 17
#define FLEXE_OAM_RX11_INT_MASK 18
#define FLEXE_OAM_TX0_INT_MASK 19
#define FLEXE_OAM_TX1_INT_MASK 20
#define FLEXE_OAM_TX2_INT_MASK 21
#define SAR_RX_INT_MASK 22
#define SAR_TX_INT_MASK 23
#define SAR_OAM_RX0_INT_MASK 24
#define SAR_OAM_RX2_INT_MASK 25
#define SAR_OAM_RX3_INT_MASK 26
#define SAR_OAM_RX4_INT_MASK 27
#define SAR_OAM_RX5_INT_MASK 28
#define SAR_OAM_RX6_INT_MASK 29
#define SAR_OAM_RX7_INT 0
#define SAR_OAM_RX8_INT 1
#define SAR_OAM_RX9_INT 2
#define SAR_OAM_RX10_INT 3
#define SAR_OAM_RX11_INT 4
#define SAR_OAM_TX0_INT 5
#define SAR_OAM_TX1_INT 6
#define SAR_OAM_TX2_INT 7
#define IEEE_1588_INT 8
#define SAR_OAM_RX13_INT 9
#define FLEXE_OAM_RX13_INT 10
#define SAR_OAM_RX14_INT 11
#define FLEXE_OAM_RX14_INT 12
#define SAR_OAM_RX15_INT 13
#define FLEXE_OAM_RX15_INT 14
#define PORT_BUFFER0_INT 15
#define PORT_BUFFER1_INT 16
#define PORT_BUFFER2_INT 17
#define PORT_BUFFER3_INT 18
#define PORT_BUFFER4_INT 19
#define PORT_BUFFER5_INT 20
#define PORT_BUFFER6_INT 21
#define PORT_BUFFER7_INT 22
#define SAR_OAM_RX7_INT_MASK 0
#define SAR_OAM_RX8_INT_MASK 1
#define SAR_OAM_RX9_INT_MASK 2
#define SAR_OAM_RX10_INT_MASK 3
#define SAR_OAM_RX11_INT_MASK 4
#define SAR_OAM_TX0_INT_MASK 5
#define SAR_OAM_TX1_INT_MASK 6
#define SAR_OAM_TX2_INT_MASK 7
#define IEEE_1588_INT_MASK 8
#define SAR_OAM_RX13_INT_MASK 9
#define FLEXE_OAM_RX13_INT_MASK 10
#define SAR_OAM_RX14_INT_MASK 11
#define FLEXE_OAM_RX14_INT_MASK 12
#define SAR_OAM_RX15_INT_MASK 13
#define FLEXE_OAM_RX15_INT_MASK 14
#define PORT_BUFFER0_INT_MASK 15
#define PORT_BUFFER1_INT_MASK 16
#define PORT_BUFFER2_INT_MASK 17
#define PORT_BUFFER3_INT_MASK 18
#define PORT_BUFFER4_INT_MASK 19
#define PORT_BUFFER5_INT_MASK 20
#define PORT_BUFFER6_INT_MASK 21
#define PORT_BUFFER7_INT_MASK 22

/******************************************************************************
*
* MOUDLE_NAME
*
*	 time_sync
*
* MOUDEL_ADDRESS
*
*	 time_sync
*
* MOUDLE_OFFSET
*
*    time_sync
*
* MOUDLE_RegisterName
*
*	 time_sync
*
*
******************************************************************************/
/************time_sync*****************/
#define TIME_SYNC_EN 0
#define OFFSET_SECOND 0
#define OFFSET_NS 0
#define OFFSET_SIGN 1
#define DRIFT_FRAC 0
#define DRIFT_SIGN 1
#define TX_DUTY_CYCLE 0
#define TX_GPS_DIFF 0
#define CRC_ERR_CNT 0
#define PPS_CNT 0
#define INF_FRM_CNT 0
#define STA_FRM_CNT 0
#define TIME_CAP_EN 0
#define REAL_REG_TURN 1
#define TS_WR_FULL 0
#define TIME_CAP_COMPLETE 1
#define RX_UTC_TIME 0
#define RX_LEAPS 0
#define RX_SEC_STA 0
#define RX_TACC 0
#define RX_SRC_TYPE 0
#define RX_SRC_STA 0
#define RX_SRC_ALM 0
#define RX_PPS_LOS 0
#define CAPTURE_NS 0
#define CAPTURE_SEC 0
#define CAPTURE_UTC 0
#define TIME_SYNC_REAL_TIME0 0
#define TIME_SYNC_REAL_TIME1 0
#define RX_INFO_RESERVED0 0
#define RX_INFO_RESERVED1 0
#define RX_STAT_RESERVED0 0
#define RX_STAT_RESERVED1 0
#define RX_STAT_RESERVED2 0
#define ASYNC_TS_WR_FULL_INT 0
#define TIME_CAP_COMPLETE_INT 1
#define ASYNC_TS_WR_FULL_INT_MASK 0
#define TIME_CAP_COMPLETE_INT_MASK 1


/******************************************************************************
*
* MOUDLE_NAME
*
*	 bc_1588_cpu
*
* MOUDEL_ADDRESS
*
*	 bc_1588_cpu
*
* MOUDLE_OFFSET
*
*    bc_1588_cpu
*
* MOUDLE_RegisterName
*
*	 bc_1588_cpu
*
*
******************************************************************************/
/************bc_1588_cpu*****************/
#define C2P_WDATA 0
#define C2P_FINISH 0
#define P2C_FINISH 0
#define C2P_FRM_CNT 0
#define P2C_FRM_CNT 0
#define C2P_CSTATE 0
#define C2P_BANK_FULL 0
#define P2C_RDATA 0
#define P2C_CSTATE 0
#define P2C_BANK_EMPTY 0
#define C2P_DSCP_ERR 0
#define C2P_LEN_ERR 1
#define C2P_RAM_READ_PKT_ERR 2
#define P2C_PTP_ERR 0
#define P2C_LEN_ERR 1

/******************************************************************************
*
* MOUDLE_NAME
*
*	 ccu_cfg
*
* MOUDEL_ADDRESS
*
*	 ccu_cfg
*
* MOUDLE_OFFSET
*
*    ccu_cfg
*
* MOUDLE_RegisterName
*
*	 ccu_cfg
*
*
******************************************************************************/
/************ccu_cfg*****************/
#define CCU_RX_TAG_MATCH_EN 0
#define CCU_RX_PARSE_EN 1
#define CCU_RX_PTP_L2_UNI_EN 2
#define CCU_RX_CCU_PARSE_EN 3
#define CCU_RX_TC_TYPE_MODE 4
#define CCU_RX_CF_UPDATE_MODE 5
#define CCU_RX_ID_MATCH_EN 6
#define CCU_RX_DROP_ERR_CTRL 7
#define CCU_RX_FRM_FW 0
#define CCU_RX_LAYER_FW 1
#define CCU_RX_VLAN_FW 2
#define CCU_RX_MAC_FW 3
#define CCU_RX_PORT_FILTER 4
#define CCU_RX_ETH_VLAN_TAG 0
#define CCU_RX_ETH_CVLAN_TAG 1
#define CCU_RX_L2_ETH_TYPE 0
#define CCU_RX_PTP_VERSION 1
#define CCU_RX_IPV4_VERSION 2
#define CCU_RX_IPV6_VERSION 3
#define CCU_RX_L3_IPV4_TYPE 0
#define CCU_RX_L3_IPV6_TYPE 1
#define CCU_RX_ETH_MUL_NORM_0 0
#define CCU_RX_ETH_MUL_NORM_1 1
#define CCU_RX_ETH_MUL_PEER_0 0
#define CCU_RX_ETH_MUL_PEER_1 1
#define CCU_RX_IPV4_MUL_NORM 0
#define CCU_RX_IPV4_MUL_PEER 0
#define CCU_RX_IPV6_MUL_NORM_0 0
#define CCU_RX_IPV6_MUL_NORM_1 1
#define CCU_RX_IPV6_MUL_NORM_2 2
#define CCU_RX_IPV6_MUL_NORM_3 3
#define CCU_RX_IPV6_MUL_PEER_0 0
#define CCU_RX_IPV6_MUL_PEER_1 1
#define CCU_RX_IPV6_MUL_PEER_2 2
#define CCU_RX_IPV6_MUL_PEER_3 3
#define CCU_RX_L2_SSM_TYPE 0
#define CCU_RX_L2_SSM_SUBTYPE 1
#define CCU_RX_SSM_DA_0 0
#define CCU_RX_SSM_DA_1 1
#define CCU_RX_ETH_UNI_NORM_0 0
#define CCU_RX_ETH_UNI_NORM_1 1
#define CCU_RX_IPV4_UNI_NORM 0
#define CCU_RX_IPV6_UNI_NORM_0 0
#define CCU_RX_IPV6_UNI_NORM_1 1
#define CCU_RX_IPV6_UNI_NORM_2 2
#define CCU_RX_IPV6_UNI_NORM_3 3
#define CCU_RX_UDP_PORT_UNI_NORM 0
#define CCU_RX_UDP_PORT_MUL_NORM 1
#define CCU_RX_UDP_PORT_UNI_OAM 0
#define CCU_RX_UDP_PORT_EVENT 1
#define CCU_RX_DA_0 0
#define CCU_RX_DA_1 1
#define CCU_RX_SA_0 0
#define CCU_RX_SA_1 1
#define CCU_RX_VLAN_TAG 0
#define CCU_RX_LETH_TYPE 0
#define CCU_RX_LETH_TYPE2 1
#define CCU_RX_VLAN_VID0 0
#define CCU_RX_VLAN_VID1 1
#define CCU_RX_VLAN_VID2 0
#define CCU_RX_VLAN_VID3 1
#define CCU_RX_VLAN_VID4 0
#define CCU_RX_VLAN_VID5 1
#define CCU_RX_VLAN_VID6 0
#define CCU_RX_VLAN_VID7 1
#define CCU_TX_DA_0 0
#define CCU_TX_DA_1 1
#define CCU_TX_SA_0 0
#define CCU_TX_SA_1 1
#define CCU_TX_VLAN_TAG 0
#define CCU_TX_ETH_TYPE 1
#define CCU_TX_CFG_PTP_VERSION 0
#define CCU_TX_CFG_MESSAGE 1
#define CCU_TX_CFG_TS_CTR 2
#define CCU_TX_VLAN_VID0 0
#define CCU_TX_VLAN_VID1 1
#define CCU_TX_VLAN_VID2 0
#define CCU_TX_VLAN_VID3 1
#define CCU_TX_VLAN_VID4 0
#define CCU_TX_VLAN_VID5 1
#define CCU_TX_VLAN_VID6 0
#define CCU_TX_VLAN_VID7 1
#define CCU_TX_PTP_L2_UNI_EN 0
#define CCU_TX_CFG_EN 1
#define CCU_TX_CRC_RECAL_EN 2
#define CCU_TX_CFG_DIRECTION 3
#define CCU_TX_CFG_PORT_ID 4
#define CCU_TX_TC_TYPE_MODE 5
#define CCU_TX_CF_UPDATE_MODE 6
#define CCU_TX_ETH_VLAN_TAG 0
#define CCU_TX_ETH_CVLAN_TAG 1
#define CCU_TX_L2_ETH_TYPE 0
#define CCU_TX_PTP_VERSION 1
#define CCU_TX_IPV4_VERSION 2
#define CCU_TX_IPV6_VERSION 3
#define CCU_TX_L3_IPV4_TYPE 0
#define CCU_TX_L3_IPV6_TYPE 1
#define CCU_TX_ETH_MUL_NORM_0 0
#define CCU_TX_ETH_MUL_NORM_1 1
#define CCU_TX_ETH_MUL_PEER_0 0
#define CCU_TX_ETH_MUL_PEER_1 1
#define CCU_TX_IPV4_MUL_NORM 0
#define CCU_TX_IPV4_MUL_PEER 0
#define CCU_TX_IPV6_MUL_NORM_0 0
#define CCU_TX_IPV6_MUL_NORM_1 1
#define CCU_TX_IPV6_MUL_NORM_2 2
#define CCU_TX_IPV6_MUL_NORM_3 3
#define CCU_TX_IPV6_MUL_PEER_0 0
#define CCU_TX_IPV6_MUL_PEER_1 1
#define CCU_TX_IPV6_MUL_PEER_2 2
#define CCU_TX_IPV6_MUL_PEER_3 3
#define CCU_TX_L2_SSM_TYPE 0
#define CCU_TX_L2_SSM_SUBTYPE 1
#define CCU_TX_SSM_DA_0 0
#define CCU_TX_SSM_DA_1 1
#define CCU_TX_FRM_FW 0
#define CCU_TX_LAYER_FW 1
#define CCU_TX_VLAN_FW 2
#define CCU_TX_MAC_FW 3
#define CCU_TX_PORT_FILTER 4
#define CCU_TX_ETH_UNI_NORM_0 0
#define CCU_TX_ETH_UNI_NORM_1 1
#define CCU_TX_IPV4_UNI_NORM 0
#define CCU_TX_IPV6_UNI_NORM_0 0
#define CCU_TX_IPV6_UNI_NORM_1 1
#define CCU_TX_IPV6_UNI_NORM_2 2
#define CCU_TX_IPV6_UNI_NORM_3 3
#define CCU_TX_UDP_PORT_UNI_NORM 0
#define CCU_TX_UDP_PORT_MUL_NORM 1
#define CCU_TX_UDP_PORT_UNI_OAM 0
#define CCU_TX_UDP_PORT_EVENT 1

#define CCU_RX_L2_ETH_TYPE1 0
#define CCU_TX_L2_ETH_TYPE1 1
#define CCU_RX_L2_ETH_TYPE2 0
#define CCU_TX_L2_ETH_TYPE2 1
#define CCU_RX_L2_ETH_TYPE3 0
#define CCU_TX_L2_ETH_TYPE3 1
#define CCU_RX_L2_ETH_TYPE4 0
#define CCU_TX_L2_ETH_TYPE4 1
#define CCU_RX_L2_ETH_TYPE5 0
#define CCU_TX_L2_ETH_TYPE5 1
#define CCU_RX_L2_ETH_TYPE6 0
#define CCU_TX_L2_ETH_TYPE6 1
#define CCU_RX_L2_ETH_TYPE7 0
#define CCU_TX_L2_ETH_TYPE7 1

#define CCU_RX_ETH_VLAN_TAG1 0
#define CCU_RX_ETH_CVLAN_TAG1 1
#define CCU_RX_ETH_VLAN_TAG2 0
#define CCU_RX_ETH_CVLAN_TAG2 1
#define CCU_RX_ETH_VLAN_TAG3 0
#define CCU_RX_ETH_CVLAN_TAG3 1
#define CCU_RX_ETH_VLAN_TAG4 0
#define CCU_RX_ETH_CVLAN_TAG4 1
#define CCU_RX_ETH_VLAN_TAG5 0
#define CCU_RX_ETH_CVLAN_TAG5 1
#define CCU_RX_ETH_VLAN_TAG6 0
#define CCU_RX_ETH_CVLAN_TAG6 1
#define CCU_RX_ETH_VLAN_TAG7 0
#define CCU_RX_ETH_CVLAN_TAG7 1

#define CCU_TX_ETH_VLAN_TAG1 0
#define CCU_TX_ETH_CVLAN_TAG1 1
#define CCU_TX_ETH_VLAN_TAG2 0
#define CCU_TX_ETH_CVLAN_TAG2 1
#define CCU_TX_ETH_VLAN_TAG3 0
#define CCU_TX_ETH_CVLAN_TAG3 1
#define CCU_TX_ETH_VLAN_TAG4 0
#define CCU_TX_ETH_CVLAN_TAG4 1
#define CCU_TX_ETH_VLAN_TAG5 0
#define CCU_TX_ETH_CVLAN_TAG5 1
#define CCU_TX_ETH_VLAN_TAG6 0
#define CCU_TX_ETH_CVLAN_TAG6 1
#define CCU_TX_ETH_VLAN_TAG7 0
#define CCU_TX_ETH_CVLAN_TAG7 1

#define CCU_RX_L3_IPV4_TYPE1 0
#define CCU_RX_L3_IPV6_TYPE1 1
#define CCU_RX_L3_IPV4_TYPE2 0
#define CCU_RX_L3_IPV6_TYPE2 1
#define CCU_RX_L3_IPV4_TYPE3 0
#define CCU_RX_L3_IPV6_TYPE3 1
#define CCU_RX_L3_IPV4_TYPE4 0
#define CCU_RX_L3_IPV6_TYPE4 1
#define CCU_RX_L3_IPV4_TYPE5 0
#define CCU_RX_L3_IPV6_TYPE5 1
#define CCU_RX_L3_IPV4_TYPE6 0
#define CCU_RX_L3_IPV6_TYPE6 1
#define CCU_RX_L3_IPV4_TYPE7 0
#define CCU_RX_L3_IPV6_TYPE7 1
#define CCU_TX_L3_IPV4_TYPE1 0
#define CCU_TX_L3_IPV6_TYPE1 1
#define CCU_TX_L3_IPV4_TYPE2 0
#define CCU_TX_L3_IPV6_TYPE2 1
#define CCU_TX_L3_IPV4_TYPE3 0
#define CCU_TX_L3_IPV6_TYPE3 1
#define CCU_TX_L3_IPV4_TYPE4 0
#define CCU_TX_L3_IPV6_TYPE4 1
#define CCU_TX_L3_IPV4_TYPE5 0
#define CCU_TX_L3_IPV6_TYPE5 1
#define CCU_TX_L3_IPV4_TYPE6 0
#define CCU_TX_L3_IPV6_TYPE6 1
#define CCU_TX_L3_IPV4_TYPE7 0
#define CCU_TX_L3_IPV6_TYPE7 1


/******************************************************************************
*
* MOUDLE_NAME
*
*	 ccu_rx_tx
*
* MOUDEL_ADDRESS
*
*	 ccu_rx_tx
*
* MOUDLE_OFFSET
*
*    ccu_rx_tx
*
* MOUDLE_RegisterName
*
*	 ccu_rx_tx
*
*
******************************************************************************/
/************ccu_rx_tx*****************/
#define CCU_RX_FRAME_CNT 0
#define CCU_RX_DROP_CNT 0
#define CCU_RX_FILTER_CNT 0
#define CCU_RX_FRM_ERR_CNT 0
#define CCU_RX_LEN_ERR_CNT 0
#define CCU_RX_PKT_IN_NUM 0
#define CCU_RX_SYNC_CNT 0
#define CCU_RX_DELAY_REQ_CNT 0
#define CCU_RX_PDELAY_REQ_CNT 0
#define CCU_RX_PDELAY_RESP_CNT 0
#define CCU_RX_FOLLOW_UP_CNT 0
#define CCU_RX_DELAY_RESP_CNT 0
#define CCU_RX_PDELAY_FOLLOW_CNT 0
#define CCU_RX_ANNOUNCE_CNT 0
#define CCU_RX_SIGNALING_CNT 0
#define CCU_RX_MANAGEMENT_CNT 0
#define CCU_RX_SSM_CNT 0
#define CCU_RX_SSM_REQ 0
#define CCU_RX_SSM_MESSAGE0 0
#define CCU_RX_SSM_MESSAGE1 0
#define CCU_RX_RESV_CNT 0
#define CCU_RX_DECODE_ERR_CNT 0
#define CCU_RX_DECODE_SPKT_BYTE_CNT 0
#define CCU_RX_DECODE_SPKT_NUM_CNT 0
#define CCU_RX_INPORT_FIFO_FULL 0
#define CCU_RX_OVER_LENGTH 1
#define CCU_RX_DECODE_ERR 2
#define CCU_RX_OH_BUFF_FULL 3
#define CCU_RX_DECODE_EOP_ABNOR 4
#define CCU_RX_DECODE_REMOTE_FAULT 5
#define CCU_RX_DECODE_LOCAL_FAULT 6
#define CCU_RX_LEN_MISMATCH 7
#define CCU_RX_INPUT_NO_EOP 8
#define CCU_RX_INPORT_FIFO_FULL_INT 0
#define CCU_RX_OVER_LENGTH_INT 1
#define CCU_RX_DECODE_ERR_INT 2
#define CCU_RX_OH_BUFF_FULL_INT 3
#define CCU_RX_DECODE_EOP_ABNOR_INT 4
#define CCU_RX_INPUT_NO_EOP_INT 5
#define CCU_RX_INPORT_FIFO_FULL_INT_MASK 0
#define CCU_RX_OVER_LENGTH_INT_MASK 1
#define CCU_RX_DECODE_ERR_INT_MASK 2
#define CCU_RX_OH_BUFF_FULL_INT_MASK 3
#define CCU_RX_DECODE_EOP_ABNOR_INT_MASK 4
#define CCU_RX_INPUT_NO_EOP_INT_MASK 5
#define CCU_TX_FRAME_CNT 0
#define CCU_TX_DROP_CNT 0
#define CCU_TX_FILTER_CNT 0
#define CCU_TX_ERR_CNT 0
#define CCU_TX_LEN_ERR_CNT 0
#define CCU_TX_PTP_CNT 0
#define CCU_TX_SSM_CNT 0
#define CCU_TX_SYNC_PORT_CNT 0
#define CCU_TX_DELAY_REQ_PORT_CNT 0
#define CCU_TX_PDELAY_REQ_PORT_CNT 0
#define CCU_TX_PDELAY_RESP_PORT_CNT 0
#define CCU_TX_FOLLOW_UP_PORT_CNT 0
#define CCU_TX_DELAY_RESP_PORT_CNT 0
#define CCU_TX_PDELAY_FOLLOW_UP_PORT_CNT 0
#define CCU_TX_ANNOUNCE_PORT_CNT 0
#define CCU_TX_SIGNALING_PORT_CNT 0
#define CCU_TX_MANAGEMENT_PORT_CNT 0
#define CCU_TX_RESV_PORT_CNT 0
#define CCU_TX_SSM_PORT_CNT 0
#define CCU_TX_ENCODE_ERR_CNT 0
#define CCU_TX_ENCODE_ERR_ALM 0
#define CCU_TX_64_TO_640_FIFO_FULL 1
#define CCU_TX_64_TO_640_ERR_ALM 2
#define CCU_TX_OH_BUFF_ERR_ALM 3
#define CCU_TX_ENCODE_ERR_INT 0
#define CCU_TX_64_TO_640_FIFO_FULL_INT 1
#define CCU_TX_64_TO_640_ERR_INT 2
#define CCU_TX_OH_BUFF_ERR_INT 3
#define CCU_TX_ENCODE_ERR_INT_MASK 0
#define CCU_TX_64_TO_640_FIFO_FULL_INT_MASK 1
#define CCU_TX_64_TO_640_ERR_INT_MASK 2
#define CCU_TX_OH_BUFF_ERR_INT_MASK 3
#define CCU_TX_SSM_REQ 0
#define CCU_TX_SSM_MESSAGE0 0
#define CCU_TX_SSM_MESSAGE1 0

/******************************************************************************
*
* MOUDLE_NAME
*
*	 ptp_over_oh
*
* MOUDEL_ADDRESS
*
*	 ptp_over_oh
*
* MOUDLE_OFFSET
*
*    ptp_over_oh
*
* MOUDLE_RegisterName
*
*	 ptp_over_oh
*
*
******************************************************************************/
/************ptp_over_oh*****************/
#define PTP_1588_MODE             0
#define RX_TIMESTAMP_MODE         1
#define RX_TS_SRC_MODE            2
#define RX_TS_SAMPLE_MODE         3
#define TX_TS_SRC_MODE            4
#define TX_TS_SAMPLE_MODE         5
#define CF_THRESHOLD_EN           6
#define IEEE1588_GLOBAL_TX_TEST_EN                7
#define ENABLE_RX_FAULT           8
#define IEEE1588_GLOBAL_REMOTE_FAULT_CONFIG       9
#define IEEE1588_GLOBAL_LOCAL_FAULT_CONFIG        10
#define IEEE1588_GLOBAL_MANUAL_RF_INSERT          11
#define IEEE1588_GLOBAL_MANUAL_LF_INSERT          12
#define IEEE1588_GLOBAL_MANUAL_IDLE_INSERT        13
#define IEEE1588_GLOBAL_DECODE_SEL 14
#define RX_PORT_EN 0
#define TX_PORT_EN 1
#define TX2RX_PORT_EN 2
#define PTP_GLB_INT1_INT 0
#define PTP_GLB_INT1_INT_MASK 0
#define RECALC_CRC_EN 0
#define RECALC_CKSUM_EN 1
#define TX_LEN_CTRL_EN 2
#define SAMPLE_TX_TS_EN 3
#define TX2RX_LOOP_EN 4
#define CF_FILED_SIGN 5
#define TX_ELASTIC_EN 6
#define TX_ASYM_COMP_DATA 0
#define TX_ASYM_COMP_SIGN 1
#define TX_ASYM_COMP_FRAC 2
#define CF_SET_THRESHOLD 0
#define PORT_TX_FRM_CNT 0
#define PORT_TX_PTP_CNT 0
#define PORT_TX_SSM_CNT 0
#define PBUFF_DROP_CNT 0
#define PORT_2STEP_TIMESTAMP_0 0
#define PORT_2STEP_TIMESTAMP_1 1
#define TX_2STEP_CAP_READY 0
#define CF_CORRECT_ALM 1
#define PTP_TX_OH_ERR 2
#define TX_66B_CODE_ALM 3
#define PBUFF_DROP_ALM 4
#define RD_LENGTH_ALM 5
#define TX_2STEP_CAP_READY_INT 0
#define CF_CORRECT_ALM_INT 1
#define PTP_TX_OH_ERR_INT 2
#define TX_66B_CODE_ALM_INT 3
#define PBUFF_DROP_ALM_INT 4
#define RD_LENGTH_ALM_INT 5
#define TX_2STEP_CAP_READY_INT_MASK 0
#define CF_CORRECT_ALM_INT_MASK 1
#define PTP_TX_OH_ERR_INT_MASK 2
#define TX_66B_CODE_ALM_INT_MASK 3
#define PBUFF_DROP_ALM_INT_MASK 4
#define RD_LENGTH_ALM_INT_MASK 5
#define PORT_BUFF_FULL 0
#define PORT_BUFF_EMPTY 1
#define TX_BBUFF_BANK_ST 2
#define ERROR_DROP_EN 0
#define RX_COMP_FRAC 0
#define RX_COMP_NS 1
#define RX_COMP_SIGN 2
#define RX_FRM_CNT 0
#define CTC_PORT_BUFF_FULL 0
#define CTC_PORT_BUFF_EMPTY 1
#define RX_BBUFF_BANK_ST 2
#define BK_FRM_ALL_CNT 0
#define BK_FRM_ERR_CNT 0
#define BK_DROP_FRM_CNT 0
#define RX_PB_BUFF_FULL_ALM 0
#define PTP_RX_OH_ERR 1
#define RX_66B_CODE_ALM 2
#define RX_CAP_TS_READY 3
#define PRD_LENGTH_ALM 4
#define RX_PB_BUFF_FULL_ALM_INT 0
#define PTP_RX_OH_ERR_INT 1
#define RX_66B_CODE_ALM_INT 2
#define RX_CAP_TS_READY_INT 3
#define PRD_LENGTH_ALM_INT 4
#define RX_PB_BUFF_FULL_ALM_INT_MASK 0
#define PTP_RX_OH_ERR_INT_MASK 1
#define RX_66B_CODE_ALM_INT_MASK 2
#define RX_CAP_TS_READY_INT_MASK 3
#define PRD_LENGTH_ALM_INT_MASK 4
#define PORTRX_CAP_TIMESTAMP_0 0
#define PORTRX_CAP_TIMESTAMP_1 1

/******************************************************************************
*
* MOUDLE_NAME
*
* inf_ch_adp_rx	 
*
* MOUDEL_ADDRESS
*
* inf_ch_adp_rx
*
* MOUDLE_OFFSET
*
* inf_ch_adp_rx
*
* MOUDLE_RegisterName
*
* inf_ch_adp_rx
*
*
******************************************************************************/
/***********inf_ch_adp_rx************/

#define RX_MAP_TABLE 0
#define TS_DELTA 0
#define IS_400G 0
#define IS_200G 1
#define IS_PORT6 2
#define IS_ASYM 3
#define ASYM_INST_SEL 4
#define LF_SEL 0
#define RX_PORT_RESTART 0
#define FULL 0
#define FIELD_EMPTY 1


/******************************************************************************
*
* MOUDLE_NAME
*
* inf_ch_adp_tx	 
*
* MOUDEL_ADDRESS
*
* inf_ch_adp_tx
*
* MOUDLE_OFFSET
*
* inf_ch_adp_tx
*
* MOUDLE_RegisterName
*
* inf_ch_adp_tx
*
*
******************************************************************************/
/***********inf_ch_adp_tx************/

#define START_LEVEL 0
#define HIGH_LEVEL 1
#define LOW_LEVEL 2
#define INF_MAX_MIN_CLR 0
#define PROTECT_EN_0 0
#define PROTECT_EN_1 1
#define PORT_RESTART 0
#define TX_MAP_TABLE 0
#define MAP_EN 0
#define TX_MAX_LEVEL 0
#define TX_MIN_LEVEL 1
#define FULL_GB_400G 0
#define EMPTY_GB_400G 1
#define FULL_GB_200G 2
#define EMPTY_GB_200G 3
#define FULL_GB_RA 4
#define EMPTY_GB_RA 5
#define HIGH_ALM 0
#define LOW_ALM 1
#define PROTECT_UP_ALM 2
#define PROTECT_DOWN_ALM 3
#define PORT_FIFO_FULL 4
#define PORT_FIFO_EMPTY 5
#define RA_IS_400G 0
#define RA_400G_CH 1
#define TX_IS_400G 0
#define TX_IS_200G 1
#define TX_IS_BYPASS 2
#define TX_IS_PORT6 3
#define TX_IS_ASYM 4
#define TX_ASYM_INST_SEL 5
#define PROTECT_UP_LEVEL 0
#define PROTECT_DOWN_LEVEL 1
#define PORT_FIFO_FULL0_INT 0
#define PORT_FIFO_EMPTY0_INT 1
#define PROTECT_UP_ALM0_INT 2
#define PROTECT_DOWN_ALM0_INT 3
#define PORT_FIFO_FULL0_INT_MASK 0
#define PORT_FIFO_EMPTY0_INT_MASK 1
#define PROTECT_UP_ALM0_INT_MASK 2
#define PROTECT_DOWN_ALM0_INT_MASK 3
#define PORT_FIFO_FULL1_INT 0
#define PORT_FIFO_EMPTY1_INT 1
#define PROTECT_UP_ALM1_INT 2
#define PROTECT_DOWN_ALM1_INT 3
#define PORT_FIFO_FULL1_INT_MASK 0
#define PORT_FIFO_EMPTY1_INT_MASK 1
#define PROTECT_UP_ALM1_INT_MASK 2
#define PROTECT_DOWN_ALM1_INT_MASK 3
#define PORT_FIFO_FULL2_INT 0
#define PORT_FIFO_EMPTY2_INT 1
#define PROTECT_UP_ALM2_INT 2
#define PROTECT_DOWN_ALM2_INT 3
#define PORT_FIFO_FULL2_INT_MASK 0
#define PORT_FIFO_EMPTY2_INT_MASK 1
#define PROTECT_UP_ALM2_INT_MASK 2
#define PROTECT_DOWN_ALM2_INT_MASK 3
#define PORT_FIFO_FULL3_INT 0
#define PORT_FIFO_EMPTY3_INT 1
#define PROTECT_UP_ALM3_INT 2
#define PROTECT_DOWN_ALM3_INT 3
#define PORT_FIFO_FULL3_INT_MASK 0
#define PORT_FIFO_EMPTY3_INT_MASK 1
#define PROTECT_UP_ALM3_INT_MASK 2
#define PROTECT_DOWN_ALM3_INT_MASK 3
#define PORT_FIFO_FULL4_INT 0
#define PORT_FIFO_EMPTY4_INT 1
#define PROTECT_UP_ALM4_INT 2
#define PROTECT_DOWN_ALM4_INT 3
#define PORT_FIFO_FULL4_INT_MASK 0
#define PORT_FIFO_EMPTY4_INT_MASK 1
#define PROTECT_UP_ALM4_INT_MASK 2
#define PROTECT_DOWN_ALM4_INT_MASK 3
#define PORT_FIFO_FULL5_INT 0
#define PORT_FIFO_EMPTY5_INT 1
#define PROTECT_UP_ALM5_INT 2
#define PROTECT_DOWN_ALM5_INT 3
#define PORT_FIFO_FULL5_INT_MASK 0
#define PORT_FIFO_EMPTY5_INT_MASK 1
#define PROTECT_UP_ALM5_INT_MASK 2
#define PROTECT_DOWN_ALM5_INT_MASK 3
#define PORT_FIFO_FULL6_INT 0
#define PORT_FIFO_EMPTY6_INT 1
#define PROTECT_UP_ALM6_INT 2
#define PROTECT_DOWN_ALM6_INT 3
#define PORT_FIFO_FULL6_INT_MASK 0
#define PORT_FIFO_EMPTY6_INT_MASK 1
#define PROTECT_UP_ALM6_INT_MASK 2
#define PROTECT_DOWN_ALM6_INT_MASK 3
#define PORT_FIFO_FULL7_INT 0
#define PORT_FIFO_EMPTY7_INT 1
#define PROTECT_UP_ALM7_INT 2
#define PROTECT_DOWN_ALM7_INT 3
#define PORT_FIFO_FULL7_INT_MASK 0
#define PORT_FIFO_EMPTY7_INT_MASK 1
#define PROTECT_UP_ALM7_INT_MASK 2
#define PROTECT_DOWN_ALM7_INT_MASK 3

/******************************************************************************
*
* MOUDLE_NAME
*
* flexe_env 
*
* MOUDEL_ADDRESS
*
* flexe_env
*
* MOUDLE_OFFSET
*
* flexe_env
*
* MOUDLE_RegisterName
*
* flexe_env
*
*
******************************************************************************/
/***********flexe_env************/

#define CFG_PERIOD 0
#define MULTIPLIER 1
#define NOM_CNT 2
#define FLEXE_ENV_Q 0
#define ADJ_01EN 1
#define FIFO_ADJ_EN 2
#define FIFO_ADJ_PERIOD 3
#define FLEXE_ENV_EN 4
#define FIFO_ADJ_MAX_EN 5
#define IS_LOCAL 6
#define ADJ_IN_CHECK_VALUE 0
#define ADJ_IN_CORRECT_EN 1
#define FLEXE_LOCAL_EN 2
#define FLEXE_LOCAL_M 3
#define FLEXE_ENV_M 0
#define FLEXE_ENV_BASE_M 0
#define FLEXE_ENV_LOCAL_M 0
#define FLEXE_ENV_LOCAL_BASE_M 0
#define FLEXE_ADJ_IN_ALM 0
#define FLEXE_I1_OVERFLOW 0
#define FLEXE_I1_UNDERFLOW 1
#define FLEXE_I2_OVERFLOW 2
#define FLEXE_I2_UNDERFLOW 3

/******************************************************************************
*
* MOUDLE_NAME
*
* req_gen 
*
* MOUDEL_ADDRESS
*
* req_gen
*
* MOUDLE_OFFSET
*
* req_gen
*
* MOUDLE_RegisterName
*
* req_gen
*
*
******************************************************************************/
/***********req_gen************/

#define REQ_GEN_M 0
#define BASE_M 0
#define CALENDAR_EN 0
#define OHIF_1588_RATE_LIMIT_EN 1
#define EXTRA_SRC 0
#define EXTRA_DST0 1
#define EXTRA_DST1 2
#define OHIF_1588_RATIO 0
#define OHIF_1588_RATE_LIMIT_M 0
#define OHIF_1588_RATE_LIMIT_BASE_M 0
#define OHIF_1588_RATE_LIMIT_RATIO 0
#define EXTRA_M 0
#define EXTRA_BASE_M 1
#define MAC_ENV_INIT 0
#define TABLE_SWITCH 1
#define MAC_ENV_INIT_DONE 0
#define CURRENT_TABLE 1
#define BUSY 2

/******************************************************************************
*VARIABLE DEFINE
******************************************************************************/



/******************************************************************************
*LOCAL FUNCTION PROTOTYPE DECLARATION
******************************************************************************/




/******************************************************************************
*GLOBAL FUNCTION DEFINITION
******************************************************************************/

#endif



