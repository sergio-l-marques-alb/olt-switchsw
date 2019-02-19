/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */


/*
 * File:    ft.h
 * Purpose: FT definitions common to SDK and uKernel
 *
 * Notes:   Definition changes should be avoided in order to
 *          maintain compatibility between SDK and uKernel since
 *          both images are built and loaded separately.
 *
 */

#ifndef _SOC_SHARED_FT_H
#define _SOC_SHARED_FT_H

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif

/*
 * FT uC Error codes
 */
typedef enum shr_ft_uc_error_e {
    SHR_FT_UC_E_NONE      = 0,
    SHR_FT_UC_E_INTERNAL  = 1,
    SHR_FT_UC_E_MEMORY    = 2,
    SHR_FT_UC_E_UNIT      = 3,
    SHR_FT_UC_E_PARAM     = 4,
    SHR_FT_UC_E_EMPTY     = 5,
    SHR_FT_UC_E_FULL      = 6,
    SHR_FT_UC_E_NOT_FOUND = 7,
    SHR_FT_UC_E_EXISTS    = 8,
    SHR_FT_UC_E_TIMEOUT   = 9,
    SHR_FT_UC_E_BUSY      = 10,
    SHR_FT_UC_E_FAIL      = 11,
    SHR_FT_UC_E_DISABLED  = 12,
    SHR_FT_UC_E_BADID     = 13,
    SHR_FT_UC_E_RESOURCE  = 14,
    SHR_FT_UC_E_CONFIG    = 15,
    SHR_FT_UC_E_UNAVAIL   = 16,
    SHR_FT_UC_E_INIT      = 17,
    SHR_FT_UC_E_PORT      = 18
} shr_ft_uc_error_t;

/* Max flex counter pools across all devices supporting FT */
#define SHR_FT_MAX_CTR_POOLS  20

/* Key Word size */
#define SHR_FT_EM_KEY_DWORD_SIZE 32

/* Num 32 bits in EM key, only 128 bit key is supported */
#define SHR_FT_EM_KEY_MAX_DWORDS 4

/* Max number of parts of a qualifier */
#define SHR_FT_EM_KEY_MAX_QUAL_PARTS 4

/* Supported qualifiers in EM key */
typedef enum shr_ft_em_qual_e {
    SHR_FT_EM_QUAL_SRC_IPV4       = 0,
    SHR_FT_EM_QUAL_DST_IPV4       = 1,
    SHR_FT_EM_QUAL_L4_SRC_PORT    = 2,
    SHR_FT_EM_QUAL_L4_DST_PORT    = 3,
    SHR_FT_EM_QUAL_PROTOCOL       = 4,
    SHR_FT_EM_NUM_QUAL
} shr_ft_em_qual_t;


/* Max size of the IPFIX packet encap */
#define SHR_FT_MAX_COLLECTOR_ENCAP_LENGTH 128

/* Max size of the template set encap */
#define SHR_FT_MAX_TEMPLATE_SET_ENCAP_LENGTH 64

/* Max number of pipes across all devices */
#define SHR_FT_MAX_PIPES 4

/* Max number of ports across all devices in a single pipe */
#define SHR_FT_MAX_PIPE_PORTS 136

/* Max timeout for msging between SDK and UKERNEL */
#define SHR_FT_MAX_UC_MSG_TIMEOUT 5000000 /* 5 secs */

/* Group update params */
typedef enum shr_ft_group_update_e {
    SHR_FT_GROUP_UPDATE_INVALID                    = 0,
    SHR_FT_GROUP_UPDATE_FLOW_THRESHOLD             = 1,
    SHR_FT_GROUP_UPDATE_FLUSH                      = 2,
    SHR_FT_GROUP_UPDATE_COUNTER_CLEAR              = 3,
    SHR_FT_GROUP_UPDATE_OBSERVATION_DOMAIN         = 4,
    SHR_FT_GROUP_UPDATE_AGING_INTERVAL             = 5,
    SHR_FT_GROUP_UPDATE_COLLECTOR_TEMPLATE_SET     = 6,
    SHR_FT_GROUP_UPDATE_COLLECTOR_TEMPLATE_UNSET   = 7,
    SHR_FT_GROUP_UPDATE_ELPH_PROFILE               = 8,
    SHR_FT_GROUP_UPDATE_ELPH_QOS_PROFILE_ID        = 9,
    SHR_FT_GROUP_UPDATE_COUNT
} shr_ft_group_update_t;

/* List of supported Info elements that can be exported by app */
typedef enum shr_ft_template_info_elem_e {
    SHR_FT_TEMPLATE_INFO_ELEM_SRC_IPV4           = 0,
    SHR_FT_TEMPLATE_INFO_ELEM_DST_IPV4           = 1,
    SHR_FT_TEMPLATE_INFO_ELEM_L4_SRC_PORT        = 2,
    SHR_FT_TEMPLATE_INFO_ELEM_L4_DST_PORT        = 3,
    SHR_FT_TEMPLATE_INFO_ELEM_IP_PROTOCOL        = 4,
    SHR_FT_TEMPLATE_INFO_ELEM_PKT_TOTAL_COUNT    = 5,
    SHR_FT_TEMPLATE_INFO_ELEM_BYTE_TOTAL_COUNT   = 6,
    SHR_FT_TEMPLATE_INFO_ELEM_FLOW_START_TS_MSEC = 7,
    SHR_FT_TEMPLATE_INFO_ELEM_OBS_TS_MSEC        = 8,
    SHR_FT_TEMPLATE_INFO_ELEM_GROUP_ID           = 9,
    SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS
} shr_ft_template_info_elem_t;

/* Maximum number of filters (promotion + demotion) in an elephant profile */
#define SHR_FT_ELPH_MAX_FILTERS 3

#endif /* _SOC_SHARED_FT_H */
