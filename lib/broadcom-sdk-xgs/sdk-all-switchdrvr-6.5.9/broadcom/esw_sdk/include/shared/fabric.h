/*
 * $Id: fabric.h,v 1.6 Broadcom SDK $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This file defines common network port parameters.
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to define port parameters.
 */

#ifndef _SHR_FABRIC_H
#define _SHR_FABRIC_H

#define _SHR_FABRIC_LINK_STATUS_CRC_ERROR    0x00000001 /* Non-zero CRC rate */
#define _SHR_FABRIC_LINK_STATUS_SIZE_ERROR   0x00000002 /* Non-zero size
                                                          error-count */
#define _SHR_FABRIC_LINK_STATUS_CODE_GROUP_ERROR 0x00000004 /* Non-zero code group
                                                          error-count */
#define _SHR_FABRIC_LINK_STATUS_MISALIGN     0x00000008 /* Link down,
                                                          misalignment error */
#define _SHR_FABRIC_LINK_STATUS_NO_SIG_LOCK  0x00000010 /* Link down, SerDes
                                                          signal lock error */
#define _SHR_FABRIC_LINK_STATUS_NO_SIG_ACCEP 0x00000020 /* Link up, but not
                                                          accepting reachability
                                                          cells */
#define _SHR_FABRIC_LINK_STATUS_ERRORED_TOKENS 0x00000040 /* Low value, indicates
                                                          bad link connectivity
                                                          or link down, based on
                                                          reachability cells */

#define _SHR_FABRIC_LINK_NO_CONNECTIVITY (0xFFFFFFFF) /* FABRIC_LINK_NO_CONNECTIVITY */

/* bcm_fabric_priority_* flags */
#define _SHR_FABRIC_QUEUE_PRIORITY_HIGH_ONLY 0x1        
#define _SHR_FABRIC_QUEUE_PRIORITY_LOW_ONLY  0x2             
#define _SHR_FABRIC_PRIORITY_MULTICAST       0x8  
#define _SHR_FABRIC_PRIORITY_TDM             0x10

/*bcm_fabric_multicast_* flgas*/
#define _SHR_FABRIC_MULTICAST_SET_ONLY       0x1
#define _SHR_FABRIC_MULTICAST_COMMIT_ONLY    0x2
#define _SHR_FABRIC_MULTICAST_STATUS_ONLY    0x4

/*bcm_fabric_profile_threshold_* valid flags*/
#define _SHR_FABRIC_TH_FLAG_FE1_ONLY            0x01
#define _SHR_FABRIC_TH_FLAG_FE3_ONLY            0x02
#define _SHR_FABRIC_TH_FLAG_LR                  0x04
#define _SHR_FABRIC_TH_FLAG_NLR                 0x08

/* FabricLinkCellFormat flags */
#define _SHR_FABRIC_LINK_CELL_FORMAT_VSC256_V1              (0)/* VS256_V1 cell format */
#define _SHR_FABRIC_LINK_CELL_FORMAT_VSC128                 (1)/* VSC128 cell format */
#define _SHR_FABRIC_LINK_CELL_FORMAT_VSC256_V2              (2)/* VS256_V2 cell format */

/* bcm_fabric_profile_threshold_* threshold_id MACROS */

#define _SHR_FABRIC_TH_INDEX_PIPE_INDICATION           (0x1000000)
#define _SHR_FABRIC_TH_INDEX_PIPE_LEVEL_INDICATION     (0x2000000)
#define _SHR_FABRIC_TH_INDEX_PRIO_INDICATION           (0x4000000)
#define _SHR_FABRIC_TH_INDEX_CLASS_INDICATION          (0x8000000)

/* number of bits + 1, in order to represent (-1). For example we need 2 bits to represent 4 values of priority + 1 bit to represent (-1) which will be all ones  */
/*We also include a buffer space for each parameter of 3 bits for future use*/
#define _SHR_FABRIC_TH_INDEX_BUFFER_SPACE              (3)

#define _SHR_FABRIC_TH_INDEX_PIPE_NOF_BITS             (3 + _SHR_FABRIC_TH_INDEX_BUFFER_SPACE)
#define _SHR_FABRIC_TH_INDEX_LEVEL_NOF_BITS            (3 + _SHR_FABRIC_TH_INDEX_BUFFER_SPACE)
#define _SHR_FABRIC_TH_INDEX_PRIO_NOF_BITS             (3 + _SHR_FABRIC_TH_INDEX_BUFFER_SPACE)
#define _SHR_FABRIC_TH_INDEX_CAST_NOF_BITS             (2)

#define _SHR_FABRIC_TH_INDEX_PIPE_MASK                 ((1<<_SHR_FABRIC_TH_INDEX_PIPE_NOF_BITS) - 1 )
#define _SHR_FABRIC_TH_INDEX_LEVEL_MASK                ((1<<_SHR_FABRIC_TH_INDEX_LEVEL_NOF_BITS) - 1)
#define _SHR_FABRIC_TH_INDEX_PRIO_MASK                 ((1<<_SHR_FABRIC_TH_INDEX_PRIO_NOF_BITS) - 1 )
#define _SHR_FABRIC_TH_INDEX_CAST_MASK                 ((1<<_SHR_FABRIC_TH_INDEX_CAST_NOF_BITS) - 1 )

#define _BCM_FABRIC_TH_INDEX_PIPE_INDICATION        _SHR_FABRIC_TH_INDEX_PIPE_INDICATION       
#define _BCM_FABRIC_TH_INDEX_PIPE_LEVEL_INDICATION  _SHR_FABRIC_TH_INDEX_PIPE_LEVEL_INDICATION   
#define _BCM_FABRIC_TH_INDEX_PRIO_INDICATION        _SHR_FABRIC_TH_INDEX_PRIO_INDICATION    
#define _BCM_FABRIC_TH_INDEX_CLASS_INDICATION       _SHR_FABRIC_TH_INDEX_CLASS_INDICATION   

#define _BCM_FABRIC_TH_INDEX_PIPE_NOF_BITS          _SHR_FABRIC_TH_INDEX_PIPE_NOF_BITS
#define _BCM_FABRIC_TH_INDEX_LEVEL_NOF_BITS         _SHR_FABRIC_TH_INDEX_LEVEL_NOF_BITS
#define _BCM_FABRIC_TH_INDEX_PRIO_NOF_BITS          _SHR_FABRIC_TH_INDEX_PRIO_NOF_BITS
#define _BCM_FABRIC_TH_INDEX_CAST_NOF_BITS          _SHR_FABRIC_TH_INDEX_CAST_NOF_BITS

#define _BCM_FABRIC_TH_INDEX_PIPE_MASK              _SHR_FABRIC_TH_INDEX_PIPE_MASK
#define _BCM_FABRIC_TH_INDEX_LEVEL_MASK             _SHR_FABRIC_TH_INDEX_LEVEL_MASK
#define _BCM_FABRIC_TH_INDEX_PRIO_MASK              _SHR_FABRIC_TH_INDEX_PRIO_MASK
#define _BCM_FABRIC_TH_INDEX_CAST_MASK              _SHR_FABRIC_TH_INDEX_CAST_MASK

#define _BCM_FABRIC_LINK_TH_LR_ONLY                 _SHR_FABRIC_TH_FLAG_LR
#define _BCM_FABRIC_LINK_TH_NLR_ONLY                _SHR_FABRIC_TH_FLAG_NLR
#define _BCM_FABRIC_LINK_TH_FE1_LINKS_ONLY          _SHR_FABRIC_TH_FLAG_FE1_ONLY
#define _BCM_FABRIC_LINK_TH_FE3_LINKS_ONLY          _SHR_FABRIC_TH_FLAG_FE3_ONLY


#define _SHR_FABRIC_TH_INDEX_PIPE_SET(pipe)     \
                    (   _SHR_FABRIC_TH_INDEX_PIPE_INDICATION \
                      | ( (pipe) & _BCM_FABRIC_TH_INDEX_PIPE_MASK) \
                    )

#define _SHR_FABRIC_TH_INDEX_PIPE_LEVEL_SET(pipe, level)     \
                    (   _SHR_FABRIC_TH_INDEX_PIPE_LEVEL_INDICATION                                                   \
                      | (( (pipe) & _BCM_FABRIC_TH_INDEX_PIPE_MASK)  << _BCM_FABRIC_TH_INDEX_LEVEL_NOF_BITS  )       \
                      | (( level) & _BCM_FABRIC_TH_INDEX_LEVEL_MASK )                                                 \
                    )

#define _SHR_FABRIC_TH_INDEX_PRIO_SET(prio)     \
                    (   _SHR_FABRIC_TH_INDEX_PRIO_INDICATION            \
                      | (( (prio) & _BCM_FABRIC_TH_INDEX_PRIO_MASK) )   \
                    )

#define _SHR_FABRIC_TH_INDEX_CAST_PRIO_SET(cast, prio)     \
                    (   _BCM_FABRIC_TH_INDEX_CLASS_INDICATION                                                     \
                      | (( (cast) & _BCM_FABRIC_TH_INDEX_CAST_MASK)  << _BCM_FABRIC_TH_INDEX_PRIO_NOF_BITS  )         \
                      | (( (prio) & _BCM_FABRIC_TH_INDEX_PRIO_MASK) )                                             \
                    )


/* Fabric threshold ID */
typedef uint32 _shr_dnxf_threshold_id_t;

/* fabric_threshold_type */
typedef enum _shr_dnxf_threshold_type_e {
    _SHR_FABRIC_MID_TAG_DROP_CLASS_TH = 0,     /* INGRESS ADMISSION THRESHOLDS */
    _SHR_FABRIC_TX_LINK_LOAD_DROP_PIPE_TH = 1,
    _SHR_FABRIC_TX_FRAG_GUARANTEED_PIPE_TH = 2,
    _SHR_FABRIC_SHARED_DROP_CLASS_TH = 3,
    _SHR_FABRIC_SHARED_MC_COPIES_DROP_PRIO_TH = 4,
    _SHR_FABRIC_MID_FRAG_DROP_CLASS_TH = 5,
    _SHR_FABRIC_SHARED_BANK_MC_DROP_PRIO_TH = 6, /* DTL ADMISSION THRESHOLDS */
    _SHR_FABRIC_TX_TAG_DROP_CLASS_TH = 7,
    _SHR_FABRIC_TX_FRAG_DROP_CLASS_TH = 8,
    _SHR_FABRIC_TX_TAG_GCI_PIPE_TH = 9,      /* GCI GENERATION */
    _SHR_FABRIC_TX_FRAG_GCI_PIPE_TH = 10, 
    _SHR_FABRIC_MID_TAG_GCI_PIPE_TH = 11,
    _SHR_FABRIC_MID_FRAG_GCI_PIPE_TH = 12,
    _SHR_FABRIC_SHARED_GCI_PIPE_TH = 13,
    _SHR_FABRIC_SHARED_FRAG_COPIES_GCI_PIPE_TH = 14,
    _SHR_FABRIC_TX_FRAG_GUARANTEED_RCI_PIPE_TH = 15, /* RCI GENERATION */
    _SHR_FABRIC_TX_LINK_LOAD_RCI_PIPE_TH = 16,
    _SHR_FABRIC_TX_TAG_RCI_PIPE_TH = 17,
    _SHR_FABRIC_TX_FRAG_RCI_PIPE_TH = 18,
    _SHR_FABRIC_MID_TAG_RCI_PIPE_TH = 19,
    _SHR_FABRIC_MID_FRAG_RCI_PIPE_TH = 20,
    _SHR_FABRIC_SHARED_RCI_PIPE_TH = 21,
    _SHR_FABRIC_MID_MC_COPIES_FC_PRIO_TH = 22,  /* FC GENERATION */
    _SHR_FABRIC_MID_TAG_FC_PIPE_TH = 23,
    _SHR_FABRIC_MID_FRAG_FC_PIPE_TH = 24,
    _SHR_FABRIC_SHARED_FC_PIPE_TH = 25,
    _SHR_FABRIC_TX_TAG_FC_PIPE_TH = 26,
    _SHR_FABRIC_TX_FRAG_FC_PIPE_TH = 27,
    _SHR_FABRIC_MID_MASK_FC_PIPE_TH = 28,
    _SHR_FABRIC_TX_FE1_BYPASS_LLFCFC_PIPE_TH = 29,
    _SHR_FABRIC_TX_FIFO_SIZE_PIPE_TH = 30,         /* FIFO SIZES */
    _SHR_FABRIC_MID_FIFO_SIZE_PIPE_TH = 31,
    _SHR_FABRIC_RX_FRAG_DROP_PIPE_TH = 32,     /* DCH LEGACY THRESHOLDS */
    _SHR_FABRIC_RX_FRAG_GCI_PIPE_TH = 33,
    _SHR_FABRIC_RX_LLFCFC_PIPE_TH = 34,
    _SHR_FABRIC_RX_FIFO_SIZE_PIPE_TH = 35,
    _SHR_FABRIC_RX_WFQ_DYNAMIC_WEIGHT_PIPE_TH = 36, /* Dynamic weights*/
    _SHR_FABRIC_MID_WFQ_DYNAMIC_WEIGHT_PIPE_TH = 37,
    _SHR_FABRIC_TX_WFQ_DYNAMIC_WEIGHT_PIPE_TH = 38,
    _SHR_FABRIC_THREHOLDS_COUNT = 39 /*This value should always be last*/
} _shr_dnxf_threshold_type_t;

#define _SHR_FABRIC_PIPE_ALL (-1)

#endif	/* !_SHR_FABRIC_H */

