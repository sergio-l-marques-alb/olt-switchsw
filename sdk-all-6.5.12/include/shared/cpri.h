/*
 * $Id: port.h,v 1.120 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This file defines commo cpri port  parameters.
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to define cpri  parameters.
 */

#ifndef _SHR_CPRI_H
#define _SHR_CPRI_H

typedef struct _shr_cpri_cw_l1_protocol_info_s{
  uint32 protocol_ver; /* */
  uint32 hdlc_rate; /*  */
  int reset; /* DL request, UL ack */
  int l1_lof;
  int l1_los;
  int l1_sdi;
  int l1_rai;
  uint32  eth_pointer;  /* Eth control location 010100 - 11111*/
 } _shr_cpri_cw_l1_protocol_info_t;

typedef enum _shr_hdlc_crc_byte_order_e {
    _shrHdlcCrcByteOrderNone = 0, /**< Default 15 .. 0  */
    _shrHdlcCrcByteOrderSwap = 1, /**<  Byte Order Swap [7:0][15:8] */
    _shrHdlcCrcByteOrderCount
} _shr_hdlc_crc_byte_order_t;

typedef enum _shr_hdlc_fcs_err_check_e {
    _shrHdlcFcsErrCheck = 0, /**<  Check for FCS Error  */
    _shrHdlcFcsErrNoCheck = 1, /**<  No FCS Error Check.  Ignore FCS. */
    _shrHdlcFcsErrCount
} _shr_hdlc_fcs_err_check_t;

typedef enum _shr_hdlc_fcs_size_e {
    _shrHdlcFcsSize8Bits = 0, /**< FCS Size 8 bits  */
    _shrHdlcFcsSize16Bits = 1, /**< FCS Size 16 bits  */
    _shrHdlcFcsSize32Bits = 2, /**< FCS Size 32 bits  */
    _shrHdlcFcsSizeCount
} _shr_hdlc_fcs_size_t;

typedef enum _shr_hdlc_filling_flag_pattern_e {
    _shrHdlcFillingFlagPattern7E = 0, /**< Fill 7E  */
    _shrHdlcFillingFlagPattern7F = 1, /**< Fill 7F  */
    _shrHdlcFillingFlagPatternFF = 2, /**< Fill FF  */
    _shrHdlcFillingFlagPatternCount
} _shr_hdlc_filling_flag_pattern_t;

typedef enum _shr_hdlc_num_flag_bytes_e {
    _shrHdlcNumFlagSizeOneByte = 0, /**< Min 1 Flag Byte  */
    _shrHdlcNumFlagSizeTwoByte = 1, /**< Min 2 Flag Bytes  */
    _shrHdlcNumFlagByteCount
} _shr_hdlc_flag_size_t;

typedef enum _shr_hdlc_crc_mode_e {
    _shrHdlcCrcAppend = 0, /**< Append CRC  */
    _shrHdlcCrcReplace = 1, /**< Replace CRC  */
    _shrHdlcCrcNoUpdate = 2, /**< Keep existing CRC.  */
    _shrHdlcCrcCount
} _shr_hdlc_crc_mode_t;

typedef enum _shr_hdlc_crc_init_val_e {
    _shrHdlcCrcInitValAll0 = 0, /**< ALL 0s  */
    _shrHdlcCrcInitValAll1 = 1, /**< All 1s  */
    _shrHdlcCrcInitValCount
} _shr_hdlc_crc_init_val_t;

typedef enum _shr_control_schan_num_bytes_e {
    _shrVsdCtrlSchanNumBytes1 = 0, /**< Sub Channel Num of Bytes 1. */
    _shrVsdCtrlSchanNumBytes2 = 1, /**< Sub Channel Num of Bytes 2. */
    _shrVsdCtrlSchanNumBytes4 = 2, /**< Sub Channel Num of Bytes 4.  */
    _shrVsdCtrlSchanNumBytesCount
} _shr_control_schan_num_bytes_t;

typedef enum _shr_control_flow_num_bytes_e {
    _shrVsdCtrlFlowNumBytes2 = 0, /**< VSD control flow num of Bytes 2. */
    _shrVsdCtrlFlowNumBytes4 = 1, /**< VSD control flow num of Bytes 4. */
    _shrVsdCtrlFlowNumBytesCount
} _shr_control_flow_num_bytes_t;

typedef struct _shr_cpri_rx_vsd_flow_info_s{
    uint32 hyper_frame_number; /**< Hyper Frame Number */
    uint32 hyper_frame_modulo; /**< Hyper Frame Modulo */
    int filter_zero_data; /**< Filer all 0's Data.  */
    uint32 section_num[4]; /**< VSD Section in this group in Order 0-127.  */
    uint32 num_sector; /**< Number of Sectors. */
    uint32 tag_id; /**< Tag Id */
} _shr_cpri_rx_vsd_flow_info_t;

typedef struct _shr_cpri_tx_vsd_flow_info_s {
    uint32 hyper_frame_number; /**< Hyper Frame Number */
    uint32 hyper_frame_modulo; /**< Hyper Frame Number */
    int repeat_mode; /**< Filer all 0's Data.  */
    uint32 section_num[4]; /**< VSD Sector in this group in Order.  */
    uint32 num_sector; /**< Number of Sectors. */
} _shr_cpri_tx_vsd_flow_info_t;

typedef enum _shr_cw_filter_mode_e {
    _shrCpriFilterDisable = 0, /**< Disable Filter */
    _shrCpriFilterNonZero = 1, /**< Non-Zero Data only */
    _shrCpriFilterPeriodic = 2, /**< Periodic Generation */
    _shrCpriFilterChange = 3, /**< Content Change Only */
    _shrCpriFilterPatternMatch = 4, /**<  Pattern Match */
    _shrCpriFilterCount
} _shr_cpri_cw_filter_mode_t;

typedef enum _shr_vsd_raw_map_mode_e {
    _shrVsdRawMapModePeriodic = 0, /**< Peridic insert data using Modulo */
    _shrVsdRawMapModeRoeFrame = 1, /**< Use Frame Number from ROE frame. */
    _shrVsdRawMapModeCount
} _shr_vsd_raw_map_mode_t;

typedef enum _shr_gcw_mask_e {
    _shrCpriGcwMaskNone = 0, /**< None of the Byte are extracted. */
    _shrCpriGcwMaskLSB = 1, /**< LSB Byte is extracted. */
    _shrCpriGcwMaskMSB = 2, /**< MSB Byte is extracted. */
    _shrCpriGcwMaskBOTH = 3, /**< Both Byte are extracted. */
    _shrCpriGcwMaskCount
} _shr_cpri_gcw_mask_t;

typedef struct _shr_cpri_cw_l1_config_info_s{
  uint32 enet_ptr;                /**< Z.194.0 Ethernet pointer p. */
  uint32 l1_fun;                  /**< Z.130.0 l1 function */
  uint32 hdlc_rate;               /**< hdlc rate */
  uint32 protocol_ver;        /**< Byte Z.2.0 version 1 or 2 */
} _shr_cpri_cw_l1_config_info_t;

/* BRCM_RSVD4 */
typedef enum _shr_control_msg_proc_type_e {
    _shrCtrlMsgProcFE = 0, /**< Multi Message Fast Ethernet. */
    _shrCtrlMsgProcFCB = 1, /**< FCB . */
    _shrCtrlMsgProcPayload = 2, /**< Message Payload Extraction. */
    _shrCtrlMsgProcFull = 3, /**< Full Message. */
    _shrCtrlMsgProcFullWithTag = 4, /**< Full Message with Tag. */
    _shrCtrlMsgProcCount
} _shr_control_msg_proc_type_t;

typedef enum _shr_tag_option_e {
    _shrTagUseDefault= 0, /**< Tag Use default. */
    _shrTagUseLookup = 1, /**< Tag Use Look up table. */
    _shrTagOptionCount
} _shr_tag_option_t;


typedef struct _shr_fast_eth_config_s {
  uint32 queue_num;
  uint32 no_fcs_err_check;
  uint32 min_packet_size;
  uint32 max_packet_size;
  int  min_packet_drop;
  int  max_packet_drop;
  int    strip_crc;
} _shr_fast_eth_config_t;

typedef enum _shr_sync_count_cycle_e {
    _shrSyncCountCycleEveryMF = 0, /**< Every Frame. */
    _shrSyncCountCycleUpToSixMF = 1, /**< Up to 6 MF . */
    _shrSyncCountCycleCount
} _shr_sync_count_cycle_t;

typedef struct _shr_tag_gen_entry_s {
    uint32 tag_id; /**< Tag ID */
    uint32 mask; /**< Mask */
    uint32 header; /**< Header Info */
    uint32 word_count; /**< Word Count, only for RTWP */
} _shr_tag_gen_entry_t;

typedef enum _shr_cpri_crc_option_e {
    _shrCrcOptionUseRoe = 0, /**< Use CRC from ROE packet. */
    _shrCrcOptionRegenerate = 1, /**< Regenerate New One . */
    _shrCrcOptionCount
} _shr_cpri_crc_option_t;


#endif  /* !_SHR_PORT_H */



