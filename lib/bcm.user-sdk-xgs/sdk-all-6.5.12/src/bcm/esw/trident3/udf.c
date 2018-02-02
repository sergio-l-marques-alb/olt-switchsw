/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * All Rights Reserved.$
 *
 * File:       field.c
 * Purpose:    BCM56870 Field Processor functions.
 */

#include <soc/defs.h>
#if defined(BCM_TRIDENT3_SUPPORT)
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/tomahawk.h>
#include <soc/field.h>
#include <soc/format.h>
#include <soc/scache.h>
#include <soc/error.h>
#include <shared/bitop.h>
#include <include/soc/mcm/formatacc.h>
#include <bcm/error.h>
#include <bcm/module.h>

#include <bcm/udf.h>
#include <bcm_int/esw/udf.h>

#if defined (BCM_WARM_BOOT_SUPPORT)
/* Warmboot Versions */
#define BCM_WB_VERSION_1_0     SOC_SCACHE_VERSION(1, 0)
#define BCM_WB_DEFAULT_VERSION BCM_WB_VERSION_1_0
#endif /* BCM_WARM_BOOT_SUPPORT */

/* Maximum number of UDF objects - (Max Chunks * number of pkt abstracts) */
#define _BCM_UDF_TD3_MAX_OBJECTS(_u_)   \
     (_BCM_UDF_CTRL_MAX_UDF_CHUNKS(_u_) * bcmUdfAbstractPktFormatLastCount)

/* UDF Object Min and Max Values. */
#define _BCM_UDF_TD3_OBJ_MIN_ID 0x1
#define _BCM_UDF_TD3_OBJ_MAX_ID 0xfffe

/* UDF Parse limit supported by SRC_PKT_OFFSET in CMD_POLICY_DATA */
#define _BCM_UDF_TD3_MAX_PARSE_LIMIT 128

/* Number of commands dedicated to UDF in CMD_POLICY_DATAf */
#define _BCM_UDF_MAX_POLICY_CMDS 8

/* Number of stages to be used to extract bytes for a given parser */
#define _BCM_UDF_MAX_PARSER_STAGES 2

/* Global UDF control structure pointers */
_bcm_udf_td3_ctrl_t *udf_td3_control[BCM_MAX_NUM_UNITS];

#define UDF_TD3_CTRL(_u_) udf_td3_control[_u_]

/*
 *  UDF Container Chunks (2 bytes each) to Destination container ID mapping.
 *
 *  UDF Chunks  Container ID
 *  ==========  ============
 *  UDF1_0      CONT_20
 *  UDF1_1      CONT_19
 *  UDF1_2      CONT_18
 *  UDF1_3      CONT_17
 *  UDF1_4      CONT_16
 *  UDF1_5      CONT_15
 *  UDF1_6      CONT_14
 *  UDF1_7      CONT_13
 *  UDF2_0      CONT_28
 *  UDF2_1      CONT_27
 *  UDF2_2      CONT_26
 *  UDF2_3      CONT_25
 *  UDF2_4      CONT_24
 *  UDF2_5      CONT_23
 *  UDF2_6      CONT_22
 *  UDF2_7      CONT_21
 */
STATIC
uint8 _bcm_udf_chunk_to_dest_cont_id_mapping[16] =
      {20, 19, 18, 17, 16, 15, 14, 13, 28, 27, 26, 25, 24, 23, 22, 21};

/* Range Check supported UDF Chunk - UDF1_CHUNK2 */
#define _BCM_UDF_TD3_RANGE_CHUNK_BMAP 0x4

/* Maximum number of Chunks Supported. */
#define _BCM_UDF_TD3_MAX_CHUNKS     16

/* Maximum Chunk Bitmap. */
#define _BCM_UDF_TD3_MAX_CHUNK_BMAP 0xffff

/*
 * Global Variable to hold Abstract
 * Packet format HW information.
 */
_bcm_udf_td3_abstr_pkt_fmt_hw_info_t *abstr_pkt_fmt_hw_info[bcmUdfAbstractPktFormatLastCount] = {NULL};

/*
 * Macro to retrieve pointer to _bcm_udf_td3_abstr_pkt_fmt_hw_info_t
 * structure for a given abstr pkt format.
 */
#define _UDF_ABSTR_PKT_FMT_HW_INFO_GET(_pkt_fmt_)   \
                 abstr_pkt_fmt_hw_info[_pkt_fmt_]


/* UDF Byte to Chunk Conversion */
#define _BCM_UDF_BYTE_TO_CHUNK(_val_) (_val_/2)

/* UDF Chunk to Byte Conversion */
#define _BCM_UDF_CHUNK_TO_BYTE(_val_) (_val_ * 2)

/* Validate Abstract Packet format */
#define _BCM_UDF_ABSTR_PKT_FORMAT_VALIDATE(_pkt_fmt_) \
    do {                                                         \
       if ((_pkt_fmt_ < bcmUdfAbstractPktFormatLlc) ||           \
           (_pkt_fmt_ >= bcmUdfAbstractPktFormatLastCount)) {    \
           return BCM_E_PARAM;                                   \
       }                                                         \
    } while (0)

/* Validate Extraction bytes for a given packet format */
#define _BCM_UDF_EXTRACTION_BYTES_VALIDATE(_pkt_format_, _bytes_) \
    do {                                                                     \
       _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *_pkt_format_hw_info_ = NULL;    \
                                                                             \
       _pkt_format_hw_info_ = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(_pkt_format_);  \
       if (_pkt_format_hw_info_ == NULL)  {                                  \
          return BCM_E_INTERNAL;                                             \
       }                                                                     \
       if (_bytes_ > (_pkt_format_hw_info_->max_extract_bytes -              \
                      _pkt_format_hw_info_->extracted_bytes)) {              \
          return BCM_E_CONFIG;                                               \
       }                                                                     \
    } while(0)

/* Validate chunk */
#define _BCM_UDF_CHUNK_VALIDATE(_chunk_bmap_)               \
    do {                                                    \
        if ((_chunk_bmap_ == 0x0) ||                        \
            (_chunk_bmap_ > _BCM_UDF_TD3_MAX_CHUNK_BMAP)) { \
            return BCM_E_PARAM;                             \
        }                                                   \
    } while(0)

/* Validate Chunk bitmap */
#define _BCM_UDF_OBJECT_CHUNK_IN_USE_VALIDATE(_pkt_format_, _chunk_bmap_)    \
    do {                                                                     \
       _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *_pkt_format_hw_info_ = NULL;    \
                                                                             \
       _pkt_format_hw_info_ = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(_pkt_format_);  \
       if (_pkt_format_hw_info_ == NULL)  {                                  \
          return BCM_E_INTERNAL;                                             \
       }                                                                     \
       if (_pkt_format_hw_info_->chunk_bmap_used & _chunk_bmap_) {           \
          return BCM_E_CONFIG;                                               \
       }                                                                     \
    } while(0)

/*
 * Typedef:
 *    _BCM_UDF_ABSTRACT_PKT_FORMAT_MAPPING_DECL
 */
#define _BCM_UDF_ABSTRACT_PKT_FORMAT_MAPPING_DECL    \
    int _rv_;                                        \
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t _pkt_format_hw_cfg_;

/*
 * Typedef:
 *    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG
 * Purpose:
 *    Insert UDF abstract packet format configuration.
 */
#define _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(_unit_, _pkt_format_, _parser_, _offset_type_, \
                                            _mem1_, _index1_, _mem2_, _index2_, _e_bytes_) \
      do {                                                                     \
            sal_memset(&_pkt_format_hw_cfg_, 0x0,                              \
                           sizeof(_bcm_udf_td3_abstr_pkt_fmt_hw_info_t));      \
            (_pkt_format_hw_cfg_).parser = (_parser_);                         \
            (_pkt_format_hw_cfg_).base_offset = _offset_type_;                 \
            (_pkt_format_hw_cfg_).stage_policy_mem1   = (_mem1_);              \
            (_pkt_format_hw_cfg_).stage_policy_mem2   = (_mem2_);              \
            (_pkt_format_hw_cfg_).hfe_profile_ptr1  = (_index1_);              \
            (_pkt_format_hw_cfg_).hfe_profile_ptr2  = (_index2_);              \
            (_pkt_format_hw_cfg_).max_extract_bytes   = (_e_bytes_);           \
             _rv_ = _bcm_udf_td3_abstr_pkt_fmt_hw_info_insert((_unit_),        \
                                  (_pkt_format_), &(_pkt_format_hw_cfg_));     \
             BCM_IF_ERROR_RETURN(_rv_);                                        \
         } while(0)


/*
 * Macros to manage (add, delete, get) the Object Node
 */
#define _BCM_UDF_OBJ_NODE_ADD(_u_, _new_, _head_)             \
        _UDF_DLL_NODE_ADD(_u_, _new_, _head_)

#define _BCM_UDF_OBJ_NODE_DEL(_u_, _del_, _head_)             \
    do {                                                      \
        _bcm_udf_td3_obj_info_t *_temp_;                      \
        _UDF_DLL_NODE_DEL(_u_, _del_, _head_, _temp_);        \
    } while (0)

#define _BCM_UDF_OBJ_NODE_GET(_u_, _id_, _head_, _node_)      \
    do {                                                      \
        _bcm_udf_td3_obj_info_t *_temp_;                      \
        _UDF_DLL_NODE_GET(_u_, _id_, _head_, _temp_, _node_); \
    } while (0)

#define _BCM_UDF_OBJ_NODE_DEL_ALL(_u_, _head_)                        \
    do {                                                              \
         _bcm_udf_td3_obj_info_t *_temp_;                             \
         while(_head_ != NULL) {                                      \
             _temp_ = _head_;                                         \
             _BCM_UDF_OBJ_NODE_DEL(_u_, _temp_, _head_);              \
             _BCM_UDF_FREE(_temp_);                                   \
         }                                                            \
    } while (0)

#define _BCM_UDF_OBJ_NODE_NEXT(_node_)                   \
        _node_ = _node_->prev;


soc_field_t src_pkt_offset_cmds[8] = {
                                        CMD_C_4_SRC_PKT_OFFSETf,
                                        CMD_C_5_SRC_PKT_OFFSETf,
                                        CMD_C_6_SRC_PKT_OFFSETf,
                                        CMD_C_7_SRC_PKT_OFFSETf,
                                        CMD_C_8_SRC_PKT_OFFSETf,
                                        CMD_C_9_SRC_PKT_OFFSETf,
                                        CMD_C_10_SRC_PKT_OFFSETf,
                                        CMD_C_11_SRC_PKT_OFFSETf
                                     };
soc_field_t dest_cont_id_cmds[8] = {
                                      CMD_C_4_DEST_CONT_IDf,
                                      CMD_C_5_DEST_CONT_IDf,
                                      CMD_C_6_DEST_CONT_IDf,
                                      CMD_C_7_DEST_CONT_IDf,
                                      CMD_C_8_DEST_CONT_IDf,
                                      CMD_C_9_DEST_CONT_IDf,
                                      CMD_C_10_DEST_CONT_IDf,
                                      CMD_C_11_DEST_CONT_IDf
                                   };
soc_field_t valid_cmds[8] = {
                               CMD_C_4_VALIDf,
                               CMD_C_5_VALIDf,
                               CMD_C_6_VALIDf,
                               CMD_C_7_VALIDf,
                               CMD_C_8_VALIDf,
                               CMD_C_9_VALIDf,
                               CMD_C_10_VALIDf,
                               CMD_C_11_VALIDf
                            };

#define _UDF_TD3_GET_HFE_CMD_FORMAT(_parser_, _fmt_) \
      do {                                                  \
            if (_parser_ == 1) {                            \
               _fmt_ = IP_PARSER1_HFE_CMD_POLICY_DATAfmt;   \
            } else if (_parser_ == 2) {                     \
               _fmt_ = IP_PARSER2_HFE_CMD_POLICY_DATAfmt;   \
            } else {                                        \
               _fmt_ = INVALIDfmt;                          \
            }                                               \
         } while(0)


/* Local Structure to hold chunk information assigned to UDF object. */
typedef struct _udf_obj_pkt_fmt_hw_cfg_s {
   uint8   num_chunks;                      /* Number of chunks. */
   uint8   chunk_arr[_BCM_UDF_TD3_MAX_CHUNKS];  /* Chunk array. */
   uint16  cmd_bmap_1;                      /* CMD1 Bitmap. */
   uint16  cmd_bmap_2;                      /* CMD2 Bitmap. */
} _udf_obj_pkt_fmt_hw_cfg_t;

/* Function Declarations */
#if defined (BCM_WARM_BOOT_SUPPORT)
STATIC int
_bcm_udf_td3_reinit(int unit);

STATIC int
_bcm_udf_td3_wb_alloc(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
STATIC int
_bcm_udf_td3_abstr_pkt_fmt_hw_info_insert(int unit, bcm_udf_abstract_pkt_format_t format,
                                   _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_format_info);



/*
 * Free UDF device control structure.
 */
int _bcm_udf_td3_ctrl_free(int unit, _bcm_udf_td3_ctrl_t *udf_ctrl)
{
   if (udf_ctrl == NULL) {
      return BCM_E_NONE;
   }

   if (udf_ctrl->udf_obj_head != NULL) {
      _BCM_UDF_OBJ_NODE_DEL_ALL(_u_, udf_ctrl->udf_obj_head);
      udf_ctrl->udf_obj_head = NULL;
   }

   _BCM_UDF_FREE(udf_ctrl);
   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_udf_td3_detach
 * Purpose:
 *      Detaches udf module and cleans software state.
 * Parameters:
 *      unit            - (IN)      Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_udf_td3_detach(int unit)
{
    int pkt_fmt;

    /* Free resources related to UDF module */
    BCM_IF_ERROR_RETURN(_bcm_udf_td3_ctrl_free(unit, UDF_TD3_CTRL(unit)));

    UDF_TD3_CTRL(unit) = NULL;

    /* Free Abstract Packet Format HW Info. */
    for (pkt_fmt = 0; pkt_fmt < bcmUdfAbstractPktFormatLastCount; pkt_fmt++) {
        if (abstr_pkt_fmt_hw_info[pkt_fmt] != NULL) {
           if (abstr_pkt_fmt_hw_info[pkt_fmt]->cmd_policy_data1 != NULL) {
              _BCM_UDF_FREE(abstr_pkt_fmt_hw_info[pkt_fmt]->cmd_policy_data1);
           }
  
           if (abstr_pkt_fmt_hw_info[pkt_fmt]->cmd_policy_data2 != NULL) {
              _BCM_UDF_FREE(abstr_pkt_fmt_hw_info[pkt_fmt]->cmd_policy_data2);
           }

           _BCM_UDF_FREE(abstr_pkt_fmt_hw_info[pkt_fmt]);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_udf_td3_ctrl_init
 * Purpose:
 *      Initialize UDF control and internal data structures.
 * Parameters:
 *      unit           - (IN) bcm device.
 *
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_udf_td3_ctrl_init(int unit)
{
    int alloc_sz;
    _bcm_udf_td3_ctrl_t *udf_ctrl = NULL;

    /* Detach module if already installed */
    if (UDF_TD3_CTRL(unit) != NULL) {
       _bcm_udf_td3_detach(unit);
    }

    /* Allocating memory of UDF control structure */
    alloc_sz = sizeof(_bcm_udf_td3_ctrl_t);
    _BCM_UDF_ALLOC(udf_ctrl, alloc_sz, "UDF td3 control");
    if (udf_ctrl == NULL) {
        return BCM_E_MEMORY;
    }

    UDF_TD3_CTRL(unit) = udf_ctrl;

    /* UDF Range Checker support */
    udf_ctrl->flags |= _BCM_UDF_TD3_CTRL_RANGE_CHECK;

    /*
     * Update UDF Common control fields.
     */
    UDF_CONTROL(unit)->gran = _BCM_UDF_OFFSET_GRAN2;
    UDF_CONTROL(unit)->num_chunks = _BCM_UDF_OFFSET_CHUNKS16;
    UDF_CONTROL(unit)->min_obj_id = _BCM_UDF_TD3_OBJ_MIN_ID;
    UDF_CONTROL(unit)->max_obj_id = _BCM_UDF_TD3_OBJ_MAX_ID;
    UDF_CONTROL(unit)->parse_limit = _BCM_UDF_TD3_MAX_PARSE_LIMIT;
    UDF_CONTROL(unit)->max_udfs = _BCM_UDF_TD3_MAX_OBJECTS(unit);
    return BCM_E_NONE;
}

/*
 * Initialize Abstract Packet Formats with offset and default values.
 */
int
_bcm_udf_abstr_pkt_format_info_init(int unit)
{
    int pkt_fmt;
    _BCM_UDF_ABSTRACT_PKT_FORMAT_MAPPING_DECL
    /* Validate whether UDF module is attached */
    if (UDF_TD3_CTRL(unit) == NULL) {
        return BCM_E_INIT;
    }

    for (pkt_fmt = 0; pkt_fmt < bcmUdfAbstractPktFormatLastCount; pkt_fmt++) {
        if (abstr_pkt_fmt_hw_info[pkt_fmt] != NULL) {
           sal_free(abstr_pkt_fmt_hw_info[pkt_fmt]);
           abstr_pkt_fmt_hw_info[pkt_fmt] = NULL;
        }

        _BCM_UDF_ALLOC(abstr_pkt_fmt_hw_info[pkt_fmt],
                   (sizeof(_bcm_udf_td3_abstr_pkt_fmt_hw_info_t) *
                    bcmUdfAbstractPktFormatLastCount), "UDF Abstract Pkt Format INIT");
        if (abstr_pkt_fmt_hw_info[pkt_fmt] == NULL) {
           return BCM_E_MEMORY;
        }
    }


    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatLlc, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterLlc,
                                        INVALIDm,                       -1,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m, 21,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatInnerLlc, 2,
                                        bcmUdfPktBaseOffsetStartOfInnerLlc,
                                        INVALIDm,                       -1,
                                        IP_PARSER2_HFE_POLICY_TABLE_4m, 24,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatUnknownL3, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL3,
                                        IP_PARSER1_HFE_POLICY_TABLE_3m, 14,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m, 11,
                                        32);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatInnerUnknownL3, 2,
                                        bcmUdfPktBaseOffsetStartOfInnerL3,
                                        IP_PARSER2_HFE_POLICY_TABLE_3m, 10,
                                        IP_PARSER2_HFE_POLICY_TABLE_4m, 11,
                                        32);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatKnownNonIp, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL3,
                                        IP_PARSER1_HFE_POLICY_TABLE_3m, 16,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m, 19,
                                        32);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatInnerKnownNonIp, 2,
                                        bcmUdfPktBaseOffsetStartOfInnerL3,
                                        IP_PARSER2_HFE_POLICY_TABLE_3m, 13,
                                        IP_PARSER2_HFE_POLICY_TABLE_4m, 19,
                                        32);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatKnownL3Mim, 1,
                                        bcmUdfPktBaseOffsetStartOfTunnel,
                                        IP_PARSER1_HFE_POLICY_TABLE_2m,  6,
                                        INVALIDm,                       -1,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatKnownL3FcoeStdEncap, 1,
                                        bcmUdfPktBaseOffsetStartOfTunnel,
                                        IP_PARSER1_HFE_POLICY_TABLE_3m,  2,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m,  8,
                                        32);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatKnownL3FcoeVftIfr, 1,
                                        bcmUdfPktBaseOffsetStartOfTunnel,
                                        IP_PARSER1_HFE_POLICY_TABLE_3m,  3,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m,  7,
                                        32);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatKnownL3MplsOneLabel, 1,
                                        bcmUdfPktBaseOffsetStartOfTunnel,
                                        IP_PARSER1_HFE_POLICY_TABLE_3m, 10,
                                        INVALIDm,                       -1,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatKnownL3MplsTwoLabel, 1,
                                        bcmUdfPktBaseOffsetStartOfTunnel,
                                        IP_PARSER1_HFE_POLICY_TABLE_3m, 11,
                                        INVALIDm,                       -1,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatKnownL3MplsThreeLabel, 1,
                                        bcmUdfPktBaseOffsetStartOfTunnel,
                                        IP_PARSER1_HFE_POLICY_TABLE_3m,  5,
                                        INVALIDm,                       -1,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatKnownL3MplsFourLabel, 1,
                                        bcmUdfPktBaseOffsetStartOfTunnel,
                                        IP_PARSER1_HFE_POLICY_TABLE_3m,  6,
                                        INVALIDm,                       -1,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatKnownL3MplsMoreThanFourLabel, 1,
                                        bcmUdfPktBaseOffsetStartOfTunnel,
                                        IP_PARSER1_HFE_POLICY_TABLE_3m, 12,
                                        INVALIDm,                       -1,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatUnknownL4, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        INVALIDm,                       -1,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m, 14,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatInnerUnknownL4, 2,
                                        bcmUdfPktBaseOffsetStartOfInnerL4,
                                        INVALIDm,                       -1,
                                        IP_PARSER2_HFE_POLICY_TABLE_4m, 15,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatUdpUnknownL5, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        INVALIDm,                       -1,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m,  5,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatInnerUdpUnknownL5, 2,
                                        bcmUdfPktBaseOffsetStartOfInnerL4,
                                        INVALIDm,                       -1,
                                        IP_PARSER2_HFE_POLICY_TABLE_4m,  5,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatUdpGpeGeneve, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        IP_PARSER1_HFE_POLICY_TABLE_2m, 19,
                                        INVALIDm,                       -1,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatUdpBfd, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        INVALIDm,                       -1,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m,  2,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatInnerUdpBfd, 2,
                                        bcmUdfPktBaseOffsetStartOfInnerL4,
                                        INVALIDm,                       -1,
                                        IP_PARSER2_HFE_POLICY_TABLE_4m,  2,
                                        16);
    
#if 0
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatUdpBfdUnknown, 1,
                                        INVALIDm,                       -1,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m, 24,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatInnerUdpBfdUnknown, 2,
                                        INVALIDm,                       -1,
                                        IP_PARSER2_HFE_POLICY_TABLE_4m, 14,
                                        16);
#endif
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatUdpVxlan, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        INVALIDm,                       -1,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m,  1,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatUdp1588, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        INVALIDm,                       -1,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m,  3,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatTcpUnknownL5, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL5,
                                        INVALIDm,                       -1,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m,  26,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatInnerTcpUnknownL5, 2,
                                        bcmUdfPktBaseOffsetStartOfInnerL4,
                                        INVALIDm,                       -1,
                                        IP_PARSER2_HFE_POLICY_TABLE_4m,  4,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatSctpUnknownL5, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        INVALIDm,                       -1,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m,  6,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatInnerSctpUnknownL5, 2,
                                        bcmUdfPktBaseOffsetStartOfInnerL4,
                                        INVALIDm,                       -1,
                                        IP_PARSER2_HFE_POLICY_TABLE_4m,  6,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatGreWithoutKey, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        IP_PARSER1_HFE_POLICY_TABLE_2m, 11,
                                        INVALIDm,                       -1,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatGreWithKey, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        IP_PARSER1_HFE_POLICY_TABLE_2m, 12,
                                        INVALIDm,                       -1,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatGreWithoutChecksumRouting, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        IP_PARSER1_HFE_POLICY_TABLE_2m, 10,
                                        INVALIDm,                       -1,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatL2GreWithoutChecksumRouting, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        IP_PARSER1_HFE_POLICY_TABLE_2m, 13,
                                        INVALIDm,                       -1,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatL2GreWithoutRouting, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        IP_PARSER1_HFE_POLICY_TABLE_2m, 14,
                                        INVALIDm,                       -1,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatAchNonIp, 2,
                                        bcmUdfPktBaseOffsetStartOfInnerL3,
                                        IP_PARSER2_HFE_POLICY_TABLE_3m, 14,
                                        IP_PARSER2_HFE_POLICY_TABLE_4m, 25,
                                        32);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatUdpINT, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        INVALIDm,                       -1,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m,  23,
                                        16);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CONFIG(unit, bcmUdfAbstractPktFormatTcpUnknownL5WithIpExtnHdr, 1,
                                        bcmUdfPktBaseOffsetStartOfOuterL4,
                                        INVALIDm,                       -1,
                                        IP_PARSER1_HFE_POLICY_TABLE_4m,  4,
                                        16);
    return BCM_E_NONE;
}


/*
 * Reset HFE Profile indices for all abstract pkt formats.
 */
STATIC int
_bcm_udf_td3_hfe_profile_hw_init(int unit)
{
    int rv;
    int len;
    uint8 stage;
    soc_format_t fmt;
    soc_mem_t policy_mem;
    uint8 buf[100];
    uint32 hfe_profile_ptr;
    uint8 *cmd_pol_data_buf = NULL;
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;

    for (pkt_fmt = bcmUdfAbstractPktFormatLlc;
         pkt_fmt < bcmUdfAbstractPktFormatLastCount;
         pkt_fmt++) {
        pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(pkt_fmt);
        if (pkt_fmt_hw_info == NULL) {
           continue;
        }

        /*
         * Shouldn't reset the CONTAINERS for INT, since Parsing is relied
         * on UDF chunks fixed for INT packet.
         */
        if (pkt_fmt == bcmUdfAbstractPktFormatUdpINT) {
           continue;
        }

        for (stage = 0; stage < _BCM_UDF_MAX_PARSER_STAGES; stage++) {
            if (stage == 0) {
               policy_mem = pkt_fmt_hw_info->stage_policy_mem1;
               hfe_profile_ptr = pkt_fmt_hw_info->hfe_profile_ptr1;
            } else {
               policy_mem = pkt_fmt_hw_info->stage_policy_mem2;
               hfe_profile_ptr = pkt_fmt_hw_info->hfe_profile_ptr2;
            }

            if (policy_mem == INVALIDm || policy_mem == 0) {
               continue;
            }

            _UDF_TD3_GET_HFE_CMD_FORMAT(pkt_fmt_hw_info->parser, fmt);
            if (fmt == INVALIDfmt) {
               return BCM_E_INTERNAL;
            }

            sal_memset(buf, 0x0, sizeof(buf));
            rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY,
                              hfe_profile_ptr, (uint32 *) buf);
            BCM_IF_ERROR_RETURN(rv);

            len = soc_mem_field_length(unit, policy_mem, CMD_POLICY_DATAf);
            cmd_pol_data_buf = NULL;
            _BCM_UDF_ALLOC(cmd_pol_data_buf, len, "UDF default CMD Policy data");
            if (cmd_pol_data_buf == NULL) {
               return BCM_E_MEMORY;
            }
        
            soc_mem_field_get(unit, policy_mem, (uint32 *)buf,
                          CMD_POLICY_DATAf, (uint32 *)cmd_pol_data_buf);

            if (stage == 0) {
               pkt_fmt_hw_info->cmd_policy_data1 = cmd_pol_data_buf;
            } else { 
               pkt_fmt_hw_info->cmd_policy_data2 = cmd_pol_data_buf;
            }
  
#if 0
            for (i = 0; i < _BCM_UDF_MAX_POLICY_CMDS; i++) {
                soc_format_field32_set(unit, fmt, (uint32 *)f_buf,
                                                valid_cmds[i], 0);
            }

            soc_mem_field_set(unit, policy_mem, (uint32 *)buf,
                              CMD_POLICY_DATAf, (uint32 *)f_buf);

            rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ANY,
                               hfe_profile_ptr, (uint32 *)buf);
            BCM_IF_ERROR_RETURN(rv);
#endif
            
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_udf_td3_init
 * Purpose:
 *      initializes/ReInitializes the UDF module.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_udf_td3_init(int unit)
{
    int rv;

    /* Init control structures */
    rv = _bcm_udf_td3_ctrl_init(unit);
    BCM_IF_ERROR_GOTO_CLEANUP(rv);

    /*
     * Initialize Abstract Packet Formats with offset
     * and default values.
     */
    rv = _bcm_udf_abstr_pkt_format_info_init(unit);
    BCM_IF_ERROR_GOTO_CLEANUP(rv);

#if defined (BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_udf_td3_reinit(unit);
        BCM_IF_ERROR_GOTO_CLEANUP(rv);
    } else
#endif /* BCM_WARM_BOOT_SUPPORT */
    {
        /* Reset hardware HFE Profile Indices with default values */
        rv = _bcm_udf_td3_hfe_profile_hw_init(unit);
        BCM_IF_ERROR_GOTO_CLEANUP(rv);
#if defined (BCM_WARM_BOOT_SUPPORT)
        rv = _bcm_udf_td3_wb_alloc(unit);
        BCM_IF_ERROR_GOTO_CLEANUP(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
    }

    return BCM_E_NONE;

cleanup:
    (void) _bcm_udf_td3_detach(unit);
    return rv;
}

STATIC int
_bcm_udf_td3_abstr_pkt_fmt_hw_info_insert(int unit, bcm_udf_abstract_pkt_format_t abstr_pkt_fmt,
                                          _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_format_info)
{
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info = NULL;

    if (pkt_format_info == NULL) {
       return BCM_E_INTERNAL;
    }

    pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(abstr_pkt_fmt);
    if (pkt_fmt_hw_info == NULL) {
       return BCM_E_INTERNAL;
    }

    sal_memcpy(pkt_fmt_hw_info, pkt_format_info,
               sizeof(_bcm_udf_td3_abstr_pkt_fmt_hw_info_t));

    return BCM_E_NONE;
}

/* Routine to retrieve chunks from the bitmap. */
int
_udf_chunk_bmap_to_chunks(uint32 chunk_bmap, uint8 *num_chunks, uint8 *chunk_arr)
{
    uint8 bit, i;
    uint8 num = 0;

    for (i = 0; i < _BCM_UDF_TD3_MAX_CHUNKS; i++) {
        bit = (chunk_bmap >> i) & 1;
        if (bit) {
           if (chunk_arr != NULL) {
              chunk_arr[num] = i;
           }
           num++;
        }
    }

    *num_chunks = num;

    return BCM_E_NONE;
}

/* To sanitize udf_info/pkt_format_info input structures */
STATIC int
_udf_td3_sanitize_inputs(int unit, bcm_udf_chunk_info_t *udf_info,
                         bcm_udf_alloc_hints_t *hints,
                         bcm_udf_id_t *udf_id)
{
    uint8 num_chunks = 0;

    /* NULL check */
    BCM_IF_NULL_RETURN_PARAM(udf_id);
    BCM_IF_NULL_RETURN_PARAM(udf_info);

    /* start and width should be at byte boundary */
    if (((udf_info->offset % 8) != 0) || ((udf_info->width % 8) != 0)) {
       return BCM_E_PARAM;
    }

    /* start and width should be less than max byte selection */
    if (BITS2BYTES(udf_info->offset + udf_info->width) >
                   UDF_CONTROL(unit)->parse_limit) {
       return BCM_E_PARAM;
    }

    /* check if pkt format is in valid range */
    _BCM_UDF_ABSTR_PKT_FORMAT_VALIDATE(udf_info->abstract_pkt_format);

    /* Extraction bytes (width) Validate */
    _BCM_UDF_EXTRACTION_BYTES_VALIDATE(udf_info->abstract_pkt_format,
                                       BITS2BYTES(udf_info->width));

    if (hints != NULL) {
        /* Replace flag is not supported. */
        if (hints->flags & BCM_UDF_CREATE_O_REPLACE) {
            return BCM_E_PARAM;
        }

        /* Validate UDF ID */
        if (hints->flags & BCM_UDF_CREATE_O_WITHID) {
           _BCM_UDF_ID_VALIDATE(unit, *udf_id);
        }

        /* check if chip supports Range Check */
        if (hints->flags & BCM_UDF_CREATE_O_RANGE_CHECK) {
           /* Check if device supports Range Check. */
           if (!(UDF_TD3_CTRL(unit)->flags & _BCM_UDF_TD3_CTRL_RANGE_CHECK)) {
              return BCM_E_PARAM;
           }

           /*
            * If RANGE_CHECK Flag is set, chunk bitmap should be 
            * as per the device supported chunks.
            */
           if (udf_info->chunk_bmap != _BCM_UDF_TD3_RANGE_CHUNK_BMAP) {
              return BCM_E_PARAM;
           }
        }
    }


    /* Chunk Validate */
    _BCM_UDF_CHUNK_VALIDATE(udf_info->chunk_bmap);

    /* Validate given chunk_bmap matches with width */
    _udf_chunk_bmap_to_chunks(udf_info->chunk_bmap, &num_chunks, NULL);
    if (BITS2BYTES(udf_info->width) != _BCM_UDF_CHUNK_TO_BYTE(num_chunks)) {
       return BCM_E_PARAM;
    }

    /* Check whether chunks given in the bitmap are already in use */
    _BCM_UDF_OBJECT_CHUNK_IN_USE_VALIDATE(udf_info->abstract_pkt_format,
                                          udf_info->chunk_bmap);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_udf_td3_object_info_add
 * Purpose:
 *      To convert the UDF info to object info and add to the linked list.
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      udf_info        - (IN)      UDF info to be added to linked list.
 *      object_info     - (INOUT)   Reference to allocated object info.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_udf_td3_object_info_add(int unit, bcm_udf_chunk_info_t *udf_info,
                             _bcm_udf_td3_obj_info_t **obj_info)
{
    /* Allocate memory for UDF object info. */
    _BCM_UDF_ALLOC(*obj_info, sizeof(_bcm_udf_td3_obj_info_t),
                   "UDF object info.");
    if (*obj_info == NULL) {
       return BCM_E_MEMORY;
    }

    /* Insert user inputs into the object_info struct */
    (*obj_info)->width = BITS2BYTES(udf_info->width);
    (*obj_info)->offset = BITS2BYTES(udf_info->offset);
    (*obj_info)->chunk_bmap = udf_info->chunk_bmap;
    (*obj_info)->abstr_pkt_fmt = udf_info->abstract_pkt_format;
    (*obj_info)->next = NULL;
    (*obj_info)->prev = NULL;

    /* Add obj_info item to the linked list */
    _BCM_UDF_OBJ_NODE_ADD(unit, (*obj_info),
                          UDF_TD3_CTRL(unit)->udf_obj_head);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_udf_td3_object_info_delete
 * Purpose:
 *      Remove the UDF object info from the database for a given object.
 * Parameters:
 *      unit         - (IN)  Unit number.
 *      obj_info     - (IN)  Reference to allocated object info.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_udf_td3_object_info_delete(int unit, _bcm_udf_td3_obj_info_t *obj_info)
{
    if (obj_info == NULL) {
       return BCM_E_PARAM;
    }

    /* Delete obj_info from the linked list */
    _BCM_UDF_OBJ_NODE_DEL(unit, obj_info, UDF_TD3_CTRL(unit)->udf_obj_head);
    _BCM_UDF_FREE(obj_info);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_udf_td3_object_info_get
 * Purpose:
 *      Retrieves UDF object info for the given udf id.
 * Parameters:
 *      unit         - (IN)      Unit number.
 *      udf_id       - (IN)      UDF Id.
 *      obj_info     - (INOUT)   Retrieve udf object info.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_udf_td3_object_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    _bcm_udf_td3_obj_info_t **obj_info)
{
    _BCM_UDF_OBJ_NODE_GET(unit, udf_id,
                          UDF_TD3_CTRL(unit)->udf_obj_head,
                          *obj_info);

    if (*obj_info == NULL) {
       return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * To allocate running id for UDF IDs.
 */
static int
_bcm_udf_td3_object_running_id_alloc(int unit, bcm_udf_id_t *udf_id)
{
    int rv;
    int id;
    int min_id, max_id;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    min_id = UDF_CONTROL(unit)->min_obj_id;
    max_id = UDF_CONTROL(unit)->max_obj_id;

    id = _BCM_UDF_OBJ_ID_RUNNING(unit);

    if (id > max_id) {
        for (id = min_id; id <= max_id; id++) {
            rv = _bcm_udf_td3_object_info_get(unit, id, &obj_info);
            if (BCM_E_NOT_FOUND == rv) {
               break;
            }

            if (BCM_FAILURE(rv)) {
               return BCM_E_INTERNAL;
            }
        }

        /* All the udf ids are used up */
        if (id > max_id) {
           return BCM_E_FULL;
        }
    }

    *udf_id = id;

    return BCM_E_NONE;
}

static int
_udf_td3_hw_policy_cmd_alloc(
    int unit,
    _bcm_udf_td3_obj_info_t *obj_info,
    _udf_obj_pkt_fmt_hw_cfg_t *cfg_data)
{
   int i;
   uint8 num_cmds_required;
   bcm_udf_abstract_pkt_format_t pkt_fmt;
   _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *hw_info = NULL;

   pkt_fmt = obj_info->abstr_pkt_fmt;
   hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(pkt_fmt);
   if (hw_info == NULL) {
      return BCM_E_INTERNAL;
   }

   if (hw_info->chunk_bmap_used & obj_info->chunk_bmap) {
      return BCM_E_PARAM;
   }

   BCM_IF_ERROR_RETURN(_udf_chunk_bmap_to_chunks(obj_info->chunk_bmap,
                                                 &cfg_data->num_chunks,
						 cfg_data->chunk_arr));

   num_cmds_required = obj_info->width/2;

   if (hw_info->stage_policy_mem1 != INVALIDm) {
      /* Enter the loop, if CMD bitmap is not full */
      if (hw_info->valid_cmd_bmap1 != 0xff) {
         for (i = 0; i < 8; i++) {
            if (num_cmds_required == 0) {
               break;
            }

            /* Allocate CMD if the bit is not set */
            if ((hw_info->valid_cmd_bmap1 & (1 << i)) == 0) {
               cfg_data->cmd_bmap_1 |= (1 << i);
               num_cmds_required--;
            }
         }
         hw_info->valid_cmd_bmap1 |= cfg_data->cmd_bmap_1;
      }
   }

   if ((num_cmds_required != 0) && (hw_info->stage_policy_mem2 != INVALIDm)) {
      /* Enter the loop, if CMD bitmap is not full */
      if (hw_info->valid_cmd_bmap2 != 0xff) {
         for (i = 0; i < 8; i++) {
            if (num_cmds_required == 0) {
               break;
            }

            /* Allocate CMD if the bit is not set */
            if ((hw_info->valid_cmd_bmap2 & (1 << i)) == 0) {
               cfg_data->cmd_bmap_2 |= (1 << i);
               num_cmds_required--;
            }
         }
         hw_info->valid_cmd_bmap2 |= cfg_data->cmd_bmap_2;
      }
   }

   if (num_cmds_required != 0) {
      return BCM_E_INTERNAL;
   }

   return BCM_E_NONE;
}


/*
 * Install HW Policy Commands for given UDF object.
 */
static int
_udf_td3_hw_policy_cmd_install(
    int unit,
    _bcm_udf_td3_obj_info_t *obj_info,
    _udf_obj_pkt_fmt_hw_cfg_t *cfg_data)
{
   int rv;
   uint8 i;
   uint8 gran = 2;
   uint8 chunk;
   uint8 stage;
   uint8 chunk_ct = 0;
   uint8 f_buf[2][50];
   uint8 buf[2][100];
   uint32 offset = 0;
   uint32 dest_cont_id;
   soc_format_t fmt;
   uint16 cmd_bmap[2];
   uint8 null_update[2] = {0};
   soc_mem_t policy_mem[2];
   uint32 hfe_profile_ptr[2];
   uint8 skip_hw_change = 0;
   bcm_udf_abstract_pkt_format_t pkt_fmt;
   _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *hw_info = NULL;

   pkt_fmt = obj_info->abstr_pkt_fmt;
   hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(pkt_fmt);

   offset = obj_info->offset;

   sal_memset(f_buf, 0x0, sizeof(f_buf));
   sal_memset(buf, 0x0, sizeof(buf));

   if (pkt_fmt == bcmUdfAbstractPktFormatUdpINT) {
      skip_hw_change = 1;
   }
 
   for (stage = 0; stage < _BCM_UDF_MAX_PARSER_STAGES; stage++) {
      if (stage == 0) {
          cmd_bmap[stage] = cfg_data->cmd_bmap_1;
          policy_mem[stage] = hw_info->stage_policy_mem1;
          hfe_profile_ptr[stage] = hw_info->hfe_profile_ptr1;
      } else {
          cmd_bmap[stage] = cfg_data->cmd_bmap_2;
          policy_mem[stage] = hw_info->stage_policy_mem2;
          hfe_profile_ptr[stage] = hw_info->hfe_profile_ptr2;
      }

      if ((policy_mem[stage] == INVALIDm) || (skip_hw_change == 1)) {
         continue;
      }

      _UDF_TD3_GET_HFE_CMD_FORMAT(hw_info->parser, fmt);
      if (fmt == INVALIDfmt) {
         return BCM_E_INTERNAL;
      }

      rv = soc_mem_read(unit, policy_mem[stage], MEM_BLOCK_ANY,
                        hfe_profile_ptr[stage], (uint32 *) buf[stage]);
      BCM_IF_ERROR_RETURN(rv);

      soc_mem_field_get(unit, policy_mem[stage], (uint32 *)buf[stage],
                        CMD_POLICY_DATAf, (uint32 *)f_buf[stage]);


      for (i = 0; i < _BCM_UDF_MAX_POLICY_CMDS; i++) {
         if (cmd_bmap[stage] & (1 << i)) {
            soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                      src_pkt_offset_cmds[i], offset);
            offset += gran;

            if (chunk_ct <= cfg_data->num_chunks) {
               chunk = cfg_data->chunk_arr[chunk_ct];
               dest_cont_id = _bcm_udf_chunk_to_dest_cont_id_mapping[chunk];

               soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                     dest_cont_id_cmds[i], dest_cont_id);
               chunk_ct++;
            } else {
               return BCM_E_INTERNAL;
            }

            soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                                    valid_cmds[i], 1);
         } else if (hw_info->chunk_bmap_used == 0) {
            soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                                    valid_cmds[i], 0);
            null_update[stage] = 1;
         }
      }

      if (cmd_bmap[stage] || (null_update[stage])) {
         soc_mem_field_set(unit, policy_mem[stage], (uint32 *)buf[stage],
                        CMD_POLICY_DATAf, (uint32 *)f_buf[stage]);
      }
   }

   if (skip_hw_change == 0) {
      for (stage = 0; stage < _BCM_UDF_MAX_PARSER_STAGES; stage++) {
          if (cmd_bmap[stage] || (null_update[stage])) {
             rv = soc_mem_write(unit, policy_mem[stage], MEM_BLOCK_ANY,
                     hfe_profile_ptr[stage], (uint32 *)buf[stage]);
             BCM_IF_ERROR_RETURN(rv);
          }
      } 
   }

   obj_info->hw_cmd_bmap1 = cfg_data->cmd_bmap_1;
   obj_info->hw_cmd_bmap2 = cfg_data->cmd_bmap_2;
   hw_info->extracted_bytes += _BCM_UDF_CHUNK_TO_BYTE(cfg_data->num_chunks);
   hw_info->chunk_bmap_used |= obj_info->chunk_bmap;
   return BCM_E_NONE;
}

/*
 * Un-install HW Policy Commands for given UDF object.
 */
STATIC
int _udf_td3_hw_policy_cmd_uninstall(int unit, _bcm_udf_td3_obj_info_t *obj_info)
{
   int rv;
   uint8 stage, i;
   uint8 num_chunks = 0;
   uint8 f_buf[2][50];
   uint8 buf[2][100];
   uint8 skip_hw_init = 0;
   soc_format_t fmt;
   uint16 cmd_bmap[2];
   soc_mem_t policy_mem[2];
   uint32 hfe_profile_ptr[2];
   uint8 null_update[2] = {0};
   uint8 *cmd_policy_data = NULL;
   bcm_udf_abstract_pkt_format_t pkt_fmt;
   _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *hw_info = NULL;

   pkt_fmt = obj_info->abstr_pkt_fmt;
   hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(pkt_fmt);

   if (pkt_fmt == bcmUdfAbstractPktFormatUdpINT) {
      skip_hw_init = 1;
   }
 
   if (skip_hw_init == 0) {
      sal_memset(f_buf, 0x0, sizeof(f_buf));
      sal_memset(buf, 0x0, sizeof(buf));

      for (stage = 0; stage < _BCM_UDF_MAX_PARSER_STAGES; stage++) {
         if (stage == 0) {
             cmd_bmap[stage] = obj_info->hw_cmd_bmap1;
             policy_mem[stage] = hw_info->stage_policy_mem1;
             hfe_profile_ptr[stage] = hw_info->hfe_profile_ptr1;
             cmd_policy_data = hw_info->cmd_policy_data1;
         } else {
             cmd_bmap[stage] = obj_info->hw_cmd_bmap2;
             policy_mem[stage] = hw_info->stage_policy_mem2;
             hfe_profile_ptr[stage] = hw_info->hfe_profile_ptr2;
             cmd_policy_data = hw_info->cmd_policy_data2;
         }

         if (policy_mem[stage] == INVALIDm) {
            continue;
         }

         _UDF_TD3_GET_HFE_CMD_FORMAT(hw_info->parser, fmt);
         if (fmt == INVALIDfmt) {
            return BCM_E_INTERNAL;
         }

         if (cmd_policy_data == NULL) {
            return BCM_E_INTERNAL;
         }

         rv = soc_mem_read(unit, policy_mem[stage], MEM_BLOCK_ANY,
                           hfe_profile_ptr[stage], (uint32 *) buf[stage]);
         BCM_IF_ERROR_RETURN(rv);
         soc_mem_field_get(unit, policy_mem[stage], (uint32 *)buf[stage],
                           CMD_POLICY_DATAf, (uint32 *)f_buf[stage]);

         for (i = 0; i < _BCM_UDF_MAX_POLICY_CMDS; i++) {
            if (cmd_bmap[stage] & (1 << i)) {
               soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                                      valid_cmds[i], 0);
               soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                               dest_cont_id_cmds[i], 0);
               soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                             src_pkt_offset_cmds[i], 0);
            }
         }

         if (cmd_bmap[stage]) {
            soc_mem_field_set(unit, policy_mem[stage], (uint32 *)buf[stage],
                           CMD_POLICY_DATAf, (uint32 *)f_buf[stage]);
         }

         if (!(hw_info->chunk_bmap_used & ~(obj_info->hw_cmd_bmap1))) {
             soc_mem_field_set(unit, policy_mem[stage], (uint32 *)buf[stage],
                           CMD_POLICY_DATAf, (uint32 *)cmd_policy_data);
             null_update[stage] = 1;
         }
      }

      for (stage = 0; stage < _BCM_UDF_MAX_PARSER_STAGES; stage++) {
         if ((cmd_bmap[stage]) || (null_update[stage] == 1)) {
            rv = soc_mem_write(unit, policy_mem[stage], MEM_BLOCK_ANY,
                        hfe_profile_ptr[stage], (uint32 *)buf[stage]);
            BCM_IF_ERROR_RETURN(rv);
         }
      }
   }

   (void)_udf_chunk_bmap_to_chunks(obj_info->chunk_bmap,
                                   &num_chunks, NULL);

   /* Update Abstr Pkt Format HW variables. */
   hw_info->valid_cmd_bmap1 &= ~(obj_info->hw_cmd_bmap1);
   hw_info->valid_cmd_bmap2 &= ~(obj_info->hw_cmd_bmap2);
   hw_info->extracted_bytes -= _BCM_UDF_CHUNK_TO_BYTE(num_chunks);
   hw_info->chunk_bmap_used &= ~(obj_info->chunk_bmap);
   obj_info->hw_cmd_bmap1 = 0x0;
   obj_info->hw_cmd_bmap2 = 0x0;

   return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_udf_td3_chunk_create
 * Purpose:
 *      Creates a UDF object
 * Parameters:
 *      unit - (IN) Unit number.
 *      hints - (IN) Hints to UDF allocator
 *      udf_info - (IN) UDF Chunk Info structure
 *      udf_id - (IN/OUT) UDF ID
 * Returns:
 *      BCM_E_NONE          UDF created successfully.
 *      BCM_E_EXISTS        Entry already exists.
 *      BCM_E_FULL          UDF table full.
 *      BCM_E_UNIT          Invalid BCM Unit number.
 *      BCM_E_INIT          UDF module not initialized.
 *      BCM_E_UNAVAIL       Feature not supported.
 *      BCM_E_XXX           Standard error code.
 * Notes:
 */
int
_bcm_udf_td3_chunk_create(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_chunk_info_t  *udf_info,
    bcm_udf_id_t *udf_id)
{
    int id_running_allocated = 0;
    int rv = BCM_E_NONE;
    _udf_obj_pkt_fmt_hw_cfg_t cfg_data;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    /* UDF Device Module Init check */
    if (UDF_TD3_CTRL(unit) == NULL) {
        return BCM_E_INIT;
    }

    /* Sanitize input parameters */
    rv = _udf_td3_sanitize_inputs(unit, udf_info, hints, udf_id);
    BCM_IF_ERROR_RETURN(rv);

    /* Check if similar entry already existing */
    if ((hints != NULL) && (hints->flags & BCM_UDF_CREATE_O_WITHID)) {
        /* Retrieve UDF object info; get should be successful */
        rv = _bcm_udf_td3_object_info_get(unit, *udf_id, &obj_info);
        if (BCM_SUCCESS(rv)) {
           return BCM_E_EXISTS;
        }
    } else {
        /* Allocate new udf id */
        rv = _bcm_udf_td3_object_running_id_alloc(unit, udf_id);
        if (BCM_FAILURE(rv)) {
            return rv;
        } else {
            id_running_allocated = 1;
        }
    }

    /* Add udf info structure to linked list */
    rv = _bcm_udf_td3_object_info_add(unit, udf_info, &obj_info);
    if ((BCM_FAILURE(rv)) || (obj_info == NULL)) {
        return rv;
    }

    sal_memset(&cfg_data, 0x0, sizeof(cfg_data));
    /* Allocate udf chunks */
    rv = _udf_td3_hw_policy_cmd_alloc(unit, obj_info, &cfg_data);
    BCM_IF_ERROR_GOTO_CLEANUP(rv);

    /* Install UDF object */
    rv = _udf_td3_hw_policy_cmd_install(unit, obj_info, &cfg_data);
    BCM_IF_ERROR_GOTO_CLEANUP(rv);

    /* update udf_id in the offset_info */
    obj_info->udf_id = *udf_id;

    if (hints != NULL) {
        
        if (hints->flags & BCM_UDF_CREATE_O_FIELD_INGRESS) {
            obj_info->flags |= _BCM_UDF_TD3_OBJ_IFP;
        }
        if (hints->flags & BCM_UDF_CREATE_O_FIELD_LOOKUP) {
            obj_info->flags |= _BCM_UDF_TD3_OBJ_VFP;
        }
        if (hints->flags & BCM_UDF_CREATE_O_RANGE_CHECK) {
            obj_info->flags |= _BCM_UDF_TD3_OBJ_RANGE_CHECK;
        }
    }

cleanup:
    if (BCM_FAILURE(rv)) {
        if ((id_running_allocated == 1) &&
            (UDF_CONTROL(unit)->udf_id_running != _BCM_UDF_TD3_OBJ_MAX_ID)) {
            /* Decrement the running udf id */
            UDF_CONTROL(unit)->udf_id_running -= 1;
        }

        /* Delete the object node */
        if (obj_info != NULL) {
            _BCM_UDF_OBJ_NODE_DEL(unit, obj_info,
                                  UDF_TD3_CTRL(unit)->udf_obj_head);
            _BCM_UDF_FREE(obj_info);
        }
    }

    return rv;
}
int
_bcm_udf_td3_destroy(int unit, bcm_udf_id_t udf_id)
{
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    BCM_IF_ERROR_RETURN(_bcm_udf_td3_object_info_get(unit, udf_id, &obj_info));

    /* De-allocate HW Resource */
    BCM_IF_ERROR_RETURN(_udf_td3_hw_policy_cmd_uninstall(unit, obj_info));
    /* Detach UDF object info from its database. */
    BCM_IF_ERROR_RETURN(_bcm_udf_td3_object_info_delete(unit, obj_info));

    return BCM_E_NONE;
}

int
_bcm_udf_td3_chunk_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_chunk_info_t *udf_chunk_info)
{
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    if (udf_chunk_info == NULL) {
       return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_udf_td3_object_info_get(unit, udf_id, &obj_info));

    udf_chunk_info->offset = obj_info->offset;
    udf_chunk_info->width = obj_info->width;
    udf_chunk_info->abstract_pkt_format = obj_info->abstr_pkt_fmt;
    udf_chunk_info->chunk_bmap = obj_info->chunk_bmap;
    return BCM_E_NONE;
}

int
_bcm_udf_td3_abstr_pkt_format_obj_list_get(
    int unit,
    bcm_udf_abstract_pkt_format_t abstract_pkt_format,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual)
{
    int ct = 0;
    bcm_udf_id_t id_list[32] = {0};
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    /* Validate input params */
    if ((udf_id_list == NULL) || (actual == NULL)) {
       return BCM_E_PARAM;
    }

    /* check if pkt format is in valid range */
    _BCM_UDF_ABSTR_PKT_FORMAT_VALIDATE(abstract_pkt_format);

    obj_info = UDF_TD3_CTRL(unit)->udf_obj_head;

    while (obj_info != NULL) {
        if (obj_info->abstr_pkt_fmt == abstract_pkt_format) {
           id_list[ct++] = obj_info->udf_id;
        }
        _BCM_UDF_OBJ_NODE_NEXT(obj_info);
    }

    if (ct > max) {
       return BCM_E_PARAM;
    }

    sal_memcpy(udf_id_list, id_list, sizeof(bcm_udf_id_t) * ct);
    *actual = ct;
    return BCM_E_NONE;
}

STATIC
int _bcm_udf_td3_abstr_pkt_format_info_get(
    int unit,
    bcm_udf_abstract_pkt_format_t abstract_pkt_format,
    bcm_udf_abstract_pkt_format_info_t *pkt_format_info)
{
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;

    if (pkt_format_info == NULL) {
       return BCM_E_PARAM;
    }

    /* Validate whether UDF module is attached */
    if (UDF_TD3_CTRL(unit) == NULL) {
        return BCM_E_INIT;
    }

    /* check if pkt format is in valid range */
    _BCM_UDF_ABSTR_PKT_FORMAT_VALIDATE(abstract_pkt_format);

    pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(abstract_pkt_format);
    if (pkt_fmt_hw_info == NULL) {
       return BCM_E_INTERNAL;
    }

    pkt_format_info->base_offset = pkt_fmt_hw_info->base_offset;
    pkt_format_info->num_chunks_max =
                         _BCM_UDF_BYTE_TO_CHUNK(pkt_fmt_hw_info->max_extract_bytes);
    pkt_format_info->chunk_bmap_used = pkt_fmt_hw_info->chunk_bmap_used;

    return BCM_E_NONE;
}

STATIC int
_bcm_udf_td3_format_field_dump(int unit, soc_mem_t mem, uint32 index)
{
   uint32 buf[100]={0};
   uint32 f_buf[50]={0};
   uint32 id = 0, valid, dst_cont_id, src_offset;
   int rv;

   if (!((mem == IP_PARSER1_HFE_POLICY_TABLE_3m) || (mem == IP_PARSER1_HFE_POLICY_TABLE_4m) ||
        (mem == IP_PARSER2_HFE_POLICY_TABLE_3m) || (mem == IP_PARSER2_HFE_POLICY_TABLE_4m))) {
      return BCM_E_NONE;
   }

   rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, (uint32 *)buf);
   BCM_IF_ERROR_RETURN(rv);

   soc_mem_field_get(unit, mem, (uint32 *)buf, CMD_POLICY_DATAf, (uint32 *)f_buf);

   LOG_CLI(("ID VALID DEST_CONT_ID SRC_PKT_OFFSET\n\r"));
   LOG_CLI(("== ===== ============ ==============\n\r"));
   valid = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_0_VALIDf);
   dst_cont_id = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_0_DEST_CONT_IDf);
   src_offset = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_0_SRC_PKT_OFFSETf);
   LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_1_VALIDf);
   dst_cont_id = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_1_DEST_CONT_IDf);
   src_offset = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_1_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));


   valid = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_2_VALIDf);
   dst_cont_id = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_2_DEST_CONT_IDf);
   src_offset = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_2_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_3_VALIDf);
   dst_cont_id = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_3_DEST_CONT_IDf);
   src_offset = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_3_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_4_VALIDf);
   dst_cont_id = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_4_DEST_CONT_IDf);
   src_offset = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_4_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_5_VALIDf);
   dst_cont_id = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_5_DEST_CONT_IDf);
   src_offset = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_5_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_6_VALIDf);
   dst_cont_id = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_6_DEST_CONT_IDf);
   src_offset = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_6_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_7_VALIDf);
   dst_cont_id = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_7_DEST_CONT_IDf);
   src_offset = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_7_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_8_VALIDf);
   dst_cont_id = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_8_DEST_CONT_IDf);
   src_offset = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_8_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_9_VALIDf);
   dst_cont_id = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_9_DEST_CONT_IDf);
   src_offset = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_9_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_10_VALIDf);
   dst_cont_id = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_10_DEST_CONT_IDf);
   src_offset = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_10_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_11_VALIDf);
   dst_cont_id = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_11_DEST_CONT_IDf);
   src_offset = soc_IP_PARSER1_HFE_CMD_POLICY_DATAfmt_field32_get(unit, f_buf, CMD_C_11_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   return BCM_E_NONE;
}

STATIC
void _udf_print_in_bin(uint16 n)
{
    int ct = 0;

    while (n) {
       LOG_CLI(("%d", (n & 1)));
       n >>= 1;

       ct++;
       if (ct == 4) {
          LOG_CLI((" "));
          ct = 0;
       }
    }
}

STATIC char *
_udf_abstr_pkt_fmt_name(bcm_udf_abstract_pkt_format_t abstr_pkt_fmt)
{
    static char *text[] = BCM_UDF_ABSTRACT_PKT_FORMAT;

    if (abstr_pkt_fmt >= COUNTOF(text)) {
        return "??";
    }
    return text[abstr_pkt_fmt];
}


int
_bcm_udf_object_hw_info_dump(int unit, bcm_udf_id_t udf_id)
{
    _bcm_udf_td3_obj_info_t *obj_info = NULL;
   _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *hw_info = NULL;

    BCM_IF_ERROR_RETURN(_bcm_udf_td3_object_info_get(unit, udf_id, &obj_info));

    LOG_CLI(("UDF Object ID [%d] Information:\n\r", udf_id));
    LOG_CLI(("Offset..................%d\n\r", obj_info->offset));
    LOG_CLI(("Width...................%d\n\r", obj_info->width));
    LOG_CLI(("Abstr Pkt Fmt...........%s\n\r", _udf_abstr_pkt_fmt_name(obj_info->abstr_pkt_fmt)));
    LOG_CLI(("Chunk Bitmap............0x%x\n\r", obj_info->chunk_bmap));
    LOG_CLI(("HW CMD Bitmap1..........0x%x\n\r", obj_info->hw_cmd_bmap1));
    LOG_CLI(("HW CMD Bitmap2..........0x%x\n\r", obj_info->hw_cmd_bmap2));

    hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(obj_info->abstr_pkt_fmt);
    if (hw_info == NULL) {
       return BCM_E_INTERNAL;
    }

    LOG_CLI(("Abstr Packet Format Information:\n\r"));
    LOG_CLI(("Parser.....................%d\n\r", hw_info->parser));
    LOG_CLI(("Stage Policy Mem1..........%s\n\r", SOC_MEM_NAME(unit, hw_info->stage_policy_mem1)));
    LOG_CLI(("Stage HFE Profile Ptr1.....%d\n\r", hw_info->hfe_profile_ptr1));
    LOG_CLI(("Stage Policy Mem2..........%s\n\r", SOC_MEM_NAME(unit, hw_info->stage_policy_mem2)));
    LOG_CLI(("Stage HFE Profile Ptr2.....%d\n\r", hw_info->hfe_profile_ptr2));
    LOG_CLI(("Maximum Extr Bytes.........%d\n\r", hw_info->max_extract_bytes));
    LOG_CLI(("Used Extr Bytes............%d\n\r", hw_info->extracted_bytes));
    LOG_CLI(("Chunk Bitmap Used (bits)...0x%x (", hw_info->chunk_bmap_used));
    _udf_print_in_bin((uint16)hw_info->chunk_bmap_used);
    LOG_CLI((")\n\r"));
    LOG_CLI(("CMD Bitmap1 (bits).........0x%x (", hw_info->valid_cmd_bmap1));
    _udf_print_in_bin(hw_info->valid_cmd_bmap1);
    LOG_CLI((")\n\r"));
    LOG_CLI(("CMD Bitmap2 (bits).........0x%x (", hw_info->valid_cmd_bmap2));
    _udf_print_in_bin(hw_info->valid_cmd_bmap2);
    LOG_CLI((")\n\r"));

    if (hw_info->stage_policy_mem1 != INVALIDm) {
       _bcm_udf_td3_format_field_dump(unit, hw_info->stage_policy_mem1,
                                      hw_info->hfe_profile_ptr1);
    }
    if (hw_info->stage_policy_mem2 != INVALIDm) {
       _bcm_udf_td3_format_field_dump(unit, hw_info->stage_policy_mem2,
                                      hw_info->hfe_profile_ptr2);
    }

    return BCM_E_NONE;
}

/*
 * Routine to dump all the configured UDF objects info.
 */
STATIC void
_bcm_udf_td3_sw_dump(int unit)
{
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    if (UDF_TD3_CTRL(unit)->udf_obj_head == NULL) {
       return;
    }

    obj_info = UDF_TD3_CTRL(unit)->udf_obj_head;

    while (obj_info != NULL) {
        _bcm_udf_object_hw_info_dump(unit, obj_info->udf_id);
        _BCM_UDF_OBJ_NODE_NEXT(obj_info);
    }
}


/*
 * Routine to retrieve number of UDF objects associated
 * to the given UDF Chunk Bitmap
 */
int
_bcm_udf_td3_chunk_id_multi_get(
    int unit,
    uint32 udf_chunk_bmap,
    int max,
    int *objects_list,
    int *actual)
{
    int idx = 0;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    obj_info = UDF_TD3_CTRL(unit)->udf_obj_head;
    while (obj_info != NULL) {
        if (obj_info->chunk_bmap & udf_chunk_bmap) {
           if (idx >= max) {
              return BCM_E_PARAM;
           }
           objects_list[idx++] = obj_info->udf_id;
        }
        _BCM_UDF_OBJ_NODE_NEXT(obj_info);
    }
    *actual = idx;

    return BCM_E_NONE;
}

/*
 * Routine to return UDF chunk dedicated to Range Checker.
 */
int
_bcm_udf_td3_range_checker_chunk_info_get(
    int unit,
    uint8 *num_chunks,
    uint32 *chunk_bmap)
{
    if ((num_chunks == NULL) || (chunk_bmap == NULL)) {
       return BCM_E_PARAM;
    }

    *num_chunks = 1;
    *chunk_bmap = _BCM_UDF_TD3_RANGE_CHUNK_BMAP;
    return BCM_E_NONE; 
}

#if defined (BCM_WARM_BOOT_SUPPORT)

STATIC int
_udf_td3_wb_abstr_pkt_fmt_recover(int unit, _bcm_udf_td3_obj_info_t *obj_info)
{
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;

    pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(obj_info->abstr_pkt_fmt);
    if (pkt_fmt_hw_info == NULL) {
       return BCM_E_INTERNAL;
    }

    pkt_fmt_hw_info->extracted_bytes += obj_info->width;
    pkt_fmt_hw_info->chunk_bmap_used |= obj_info->chunk_bmap;
    pkt_fmt_hw_info->valid_cmd_bmap1 |= obj_info->hw_cmd_bmap1;
    pkt_fmt_hw_info->valid_cmd_bmap2 |= obj_info->hw_cmd_bmap2;

    return BCM_E_NONE;
}

/*
 * Routine to Recover UDF Object information during WarmBoot.
 */
STATIC int
_bcm_udf_td3_wb_obj_info_recover_1_0(int unit, uint8 **scache_ptr)
{
    _bcm_udf_td3_wb_obj_info_t *scache_obj_ptr;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;
    int ct;
    uint16 num_udfs = 0;
    uint8 num_chunks = 0;

    /* Retrieve number of UDFs */
    sal_memcpy(&num_udfs, *scache_ptr, sizeof(num_udfs));
    *scache_ptr += sizeof(num_udfs);

    /* Retrieve the UDF objects */
    scache_obj_ptr = (_bcm_udf_td3_wb_obj_info_t *)(*scache_ptr);
    for (ct = 0; ct < num_udfs; ct++) {
        /* Allocate memory for UDF object info. */
        obj_info = NULL;
        _BCM_UDF_ALLOC(obj_info, sizeof(_bcm_udf_td3_obj_info_t),
                       "UDF object info.");
        if (obj_info == NULL) {
           return BCM_E_MEMORY;
        }

        obj_info->udf_id = scache_obj_ptr->udf_id;
        obj_info->offset = scache_obj_ptr->offset;
        obj_info->chunk_bmap = scache_obj_ptr->chunk_bmap;

        (void)_udf_chunk_bmap_to_chunks(obj_info->chunk_bmap,
                                        &num_chunks, NULL);
        obj_info->width = _BCM_UDF_CHUNK_TO_BYTE(num_chunks);
        obj_info->hw_cmd_bmap1 = scache_obj_ptr->hw_cmd_bmap1;
        obj_info->hw_cmd_bmap2 = scache_obj_ptr->hw_cmd_bmap2;
        obj_info->abstr_pkt_fmt = scache_obj_ptr->abstr_pkt_fmt;

        BCM_IF_ERROR_RETURN(_udf_td3_wb_abstr_pkt_fmt_recover(unit, obj_info));

        /* Add obj_info item to the linked list */
        _BCM_UDF_OBJ_NODE_ADD(unit, (obj_info),
                              UDF_TD3_CTRL(unit)->udf_obj_head);
        scache_obj_ptr++;
    }

    *scache_ptr = (uint8 *)scache_obj_ptr;
    return BCM_E_NONE;
}

/*
 * Routine to recover UDF Abstract Pkt Fmt default cmd_policy_data.
 */
STATIC int
_bcm_udf_td3_wb_abstr_pkt_fmt_info_recover_1_0(int unit, uint8 **scache_ptr)
{
    uint8 stage, len;
    soc_mem_t policy_mem;
    uint8 buf[100];
    uint32 hfe_profile_ptr;
    uint8 *cmd_pol_data_buf = NULL;
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;

    for (pkt_fmt = bcmUdfAbstractPktFormatLlc;
         pkt_fmt < bcmUdfAbstractPktFormatLastCount;
         pkt_fmt++) {
        pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(pkt_fmt);
        if (pkt_fmt_hw_info == NULL) {
           continue;
        }

        /*
         * INT containers are fixed, need not sync the containers.
         */
        if (pkt_fmt == bcmUdfAbstractPktFormatUdpINT) {
           continue;
        }

        for (stage = 0; stage < _BCM_UDF_MAX_PARSER_STAGES; stage++) {
            if (stage == 0) {
               policy_mem = pkt_fmt_hw_info->stage_policy_mem1;
               hfe_profile_ptr = pkt_fmt_hw_info->hfe_profile_ptr1;
            } else {
               policy_mem = pkt_fmt_hw_info->stage_policy_mem2;
               hfe_profile_ptr = pkt_fmt_hw_info->hfe_profile_ptr2;
            }

            if (policy_mem == INVALIDm || policy_mem == 0) {
               continue;
            }

            len = soc_mem_field_length(unit, policy_mem, CMD_POLICY_DATAf);

            cmd_pol_data_buf = NULL;
            if (pkt_fmt_hw_info->chunk_bmap_used) {
                _BCM_UDF_ALLOC(cmd_pol_data_buf, len, "UDF WB CMD_POLICY_DATA alloc");
                if (cmd_pol_data_buf == NULL) {
                   return BCM_E_MEMORY;
                }

                sal_memcpy(cmd_pol_data_buf, *scache_ptr, len);
                *scache_ptr += len;
            } else {
                sal_memset(buf, 0x0, sizeof(buf));

                BCM_IF_ERROR_RETURN(soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY,
                                    hfe_profile_ptr, (uint32 *) buf));

                _BCM_UDF_ALLOC(cmd_pol_data_buf, len, "UDF default CMD Policy data");
                if (cmd_pol_data_buf == NULL) {
                   return BCM_E_MEMORY;
                }

                soc_mem_field_get(unit, policy_mem, (uint32 *)buf,
                             CMD_POLICY_DATAf, (uint32 *)cmd_pol_data_buf);
            }

            if (stage == 0) {
               pkt_fmt_hw_info->cmd_policy_data1 = cmd_pol_data_buf;
            } else {
               pkt_fmt_hw_info->cmd_policy_data2 = cmd_pol_data_buf;
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Routine to Sync UDF Object information for WarmBoot.
 */
STATIC int
_bcm_udf_td3_wb_obj_info_sync_1_0(int unit, uint8 **scache_ptr)
{
    _bcm_udf_td3_wb_obj_info_t *scache_obj_ptr;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;
    uint16 *scache_num_udf_ptr = NULL;
    uint16 num_udfs = 0;

    scache_num_udf_ptr = (uint16 *)(*scache_ptr);
    *scache_ptr += sizeof(num_udfs);

    scache_obj_ptr = (_bcm_udf_td3_wb_obj_info_t *)(*scache_ptr);

    obj_info = UDF_TD3_CTRL(unit)->udf_obj_head;
    while (obj_info != NULL) {
         scache_obj_ptr->udf_id = obj_info->udf_id;
         scache_obj_ptr->offset = obj_info->offset;
         scache_obj_ptr->chunk_bmap = obj_info->chunk_bmap;
         scache_obj_ptr->abstr_pkt_fmt = obj_info->abstr_pkt_fmt;
         scache_obj_ptr->hw_cmd_bmap1 = obj_info->hw_cmd_bmap1;
         scache_obj_ptr->hw_cmd_bmap2 = obj_info->hw_cmd_bmap2;

         scache_obj_ptr++;
         num_udfs++;
         _BCM_UDF_OBJ_NODE_NEXT(obj_info);
    }
    sal_memcpy(scache_num_udf_ptr, &num_udfs, sizeof(num_udfs));

    *scache_ptr = (uint8 *)scache_obj_ptr;
    return BCM_E_NONE;
}

/*
 * Routine to sync UDF Abstract Pkt Fmt default cmd_policy_data.
 */
STATIC int
_bcm_udf_td3_wb_abstr_pkt_fmt_info_sync_1_0(int unit, uint8 **scache_ptr)
{
    uint8 stage, len;
    soc_mem_t policy_mem;
    uint8 *cmd_policy_data = NULL;
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;

    for (pkt_fmt = bcmUdfAbstractPktFormatLlc;
         pkt_fmt < bcmUdfAbstractPktFormatLastCount;
         pkt_fmt++) {
        pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(pkt_fmt);
        if (pkt_fmt_hw_info == NULL) {
           continue;
        }

        /*
         * INT containers are fixed, need not sync the containers.
         */
        if (pkt_fmt == bcmUdfAbstractPktFormatUdpINT) {
           continue;
        }

        if (pkt_fmt_hw_info->chunk_bmap_used) {
            for (stage = 0; stage < _BCM_UDF_MAX_PARSER_STAGES; stage++) {
                if (stage == 0) {
                   policy_mem = pkt_fmt_hw_info->stage_policy_mem1;
                   cmd_policy_data = pkt_fmt_hw_info->cmd_policy_data1;
                } else {
                   policy_mem = pkt_fmt_hw_info->stage_policy_mem2;
                   cmd_policy_data = pkt_fmt_hw_info->cmd_policy_data2;
                }

                if (policy_mem == INVALIDm || policy_mem == 0) {
                   continue;
                }

                len = soc_mem_field_length(unit, policy_mem, CMD_POLICY_DATAf);
                sal_memcpy(*scache_ptr, cmd_policy_data, len);
                *scache_ptr += len;
            }
        } 
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_udf_td3_udf_wb_sync
 * Purpose:
 *      Syncs UDF warmboot state to scache
 * Parameters:
 *      unit   - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_udf_td3_wb_sync(int unit)
{
    uint8   *scache_ptr;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_UDF, 0);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL));

    if (UDF_CONTROL(unit) != NULL) {
       BCM_IF_ERROR_RETURN(_bcm_udf_td3_wb_obj_info_sync_1_0(unit, &scache_ptr));
       BCM_IF_ERROR_RETURN(_bcm_udf_td3_wb_abstr_pkt_fmt_info_sync_1_0(unit, &scache_ptr));
    }

    return BCM_E_NONE;
}

/* Returns required scache size in bytes for version = 1_0 */
STATIC int
_bcm_udf_td3_wb_scache_size_get_1_0(int unit, int *req_scache_size)
{
    int len;
    int alloc_size = 0;

    /* Number of UDFs */
    alloc_size += sizeof(uint16);

    /* Max UDF Object Nodes. */
    alloc_size += (UDF_CONTROL(unit)->max_udfs *
                   sizeof(_bcm_udf_td3_wb_obj_info_t));

    /* Max UDF Abstract Type CMD_POLICY_DATA. */
    len = soc_mem_field_length(unit, IP_PARSER1_HFE_POLICY_TABLE_3m, CMD_POLICY_DATAf);
    alloc_size += ((bcmUdfAbstractPktFormatLastCount - 1) * len);
 
    *req_scache_size = alloc_size;

    return BCM_E_NONE;
}

/* Returns required scache size for UDF module */
STATIC int
_bcm_udf_td3_wb_scache_size_get(int unit, int *req_scache_size)
{
    BCM_IF_ERROR_RETURN(
        _bcm_udf_td3_wb_scache_size_get_1_0(unit, req_scache_size));

    return BCM_E_NONE;
}

/* Allocates required scache size for the UDF module recovery */
STATIC int
_bcm_udf_td3_wb_alloc(int unit)
{
    int     rv;
    int     req_scache_size;
    uint8   *scache_ptr;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_UDF, 0);

    rv = _bcm_udf_td3_wb_scache_size_get(unit, &req_scache_size);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 req_scache_size, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_udf_td3_reinit
 * Purpose:
 *      Recovers UDF warmboot state from scache
 * Parameters:
 *      unit  - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_udf_td3_reinit(int unit)
{
    int     rv = BCM_E_INTERNAL;
    uint8   *scache_ptr;
    uint16  recovered_ver = 0;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_UDF, 0);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(rv);

    if (recovered_ver >= BCM_WB_VERSION_1_0) {
       rv = _bcm_udf_td3_wb_obj_info_recover_1_0(unit, &scache_ptr);
       BCM_IF_ERROR_RETURN(rv);

       rv = _bcm_udf_td3_wb_abstr_pkt_fmt_info_recover_1_0(unit, &scache_ptr);
       BCM_IF_ERROR_RETURN(rv);
    }

    if (BCM_SUCCESS(rv) &&
       (recovered_ver < BCM_WB_DEFAULT_VERSION)) {
        /*
         * This will handle the below cases
         * 1. When warboot from release which didn't had UDF module in SCACHE or
         * 2. The current release SCACHE size required is more than previous version or
         * 3. Level 1 warmboot in this case memory will not be allocated.
         */
        BCM_IF_ERROR_RETURN(_bcm_udf_td3_wb_alloc(unit));
    }

    return rv;
}

#endif /* BCM_WARM_BOOT_SUPPORT */
/*
 * Routine to dump UDF mem info.
 */
STATIC void
_bcm_udf_td3_sw_mem_dump(int unit, soc_mem_t mem, int idx)
{
    _bcm_udf_td3_format_field_dump(unit, mem, idx);
}


/*
 * Function:
 *    _bcm_udf_td3_functions_init
 *
 * Purpose:
 *    Set up functions pointers
 *
 * Parameters:
 *    functions - (IN/OUT) Pointer to device specific UDF module utility
 *                         routines.
 *
 * Returns:
 *     Nothing
 */
void
_bcm_udf_td3_functions_init(_bcm_udf_funct_t *functions)
{
    functions->udf_init                = _bcm_udf_td3_init;
    functions->udf_detach              = _bcm_udf_td3_detach;
    functions->udf_chunk_create        = _bcm_udf_td3_chunk_create;
    functions->udf_destroy             = _bcm_udf_td3_destroy;
    functions->udf_chunk_info_get      = _bcm_udf_td3_chunk_info_get;
    functions->udf_abstr_pkt_format_obj_list_get =
                                         _bcm_udf_td3_abstr_pkt_format_obj_list_get;
    functions->udf_abstr_pkt_format_info_get =
                                         _bcm_udf_td3_abstr_pkt_format_info_get;
#ifdef BCM_WARM_BOOT_SUPPORT
    functions->udf_wb_sync = _bcm_udf_td3_wb_sync;
#endif /* BCM_WARM_BOOT_SUPPORT */
    functions->udf_range_checker_chunk_info_get =
                                         _bcm_udf_td3_range_checker_chunk_info_get;
    functions->udf_sw_dump             = _bcm_udf_td3_sw_dump;
    functions->udf_sw_mem_dump         = _bcm_udf_td3_sw_mem_dump;
}
#endif /* (BCM_TRIDENT3_SUPPORT) */
