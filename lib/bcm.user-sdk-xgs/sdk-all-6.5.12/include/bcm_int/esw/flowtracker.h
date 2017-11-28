/*
 * $Id: flowtracker.h,  Exp $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        flowtracker.h
 * Purpose:     
 */
#ifndef _BCM_INT_FLOWTRACKER_H_
#define _BCM_INT_FLOWTRACKER_H_

#if defined(INCLUDE_FLOWTRACKER)

#include <soc/shared/ft.h>
#include <soc/shared/ft_msg.h>
#include <soc/profile_mem.h>
#include <shared/idxres_fl.h>
#include <bcm/flowtracker.h>

/* Group Start Id */
#define BCM_INT_FT_GROUP_START_ID 1

/* Maximum number of flow groups */
#define BCM_INT_FT_MAX_FLOW_GROUPS 255

/* Default value for max flows */
#define BCM_INT_FT_DEF_MAX_FLOWS 16384

/* Default max export length is 1500 bytes */
#define BCM_INT_FT_DEF_MAX_EXPORT_LENGTH 1500

/* Max export length supported is 9K - Jumbo */
#define BCM_INT_FT_MAX_EXPORT_LENGTH 9000

/* Maximum number of templates  */
#define BCM_INT_FT_MAX_TEMPLATES 1

/* Template start Id */
#define BCM_INT_FT_TEMPLATE_START_ID 1

/* Template end Id */
#define BCM_INT_FT_TEMPLATE_END_ID \
    (BCM_INT_FT_TEMPLATE_START_ID + BCM_INT_FT_MAX_TEMPLATES - 1)

/* Maximum number of collectors. */
#define BCM_INT_FT_MAX_COLLECTORS 1

/* Collector Start Id */
#define BCM_INT_FT_COLLECTOR_START_ID 1

/* Collector End Id */
#define BCM_INT_FT_COLLECTOR_END_ID \
    (BCM_INT_FT_COLLECTOR_START_ID + BCM_INT_FT_MAX_COLLECTORS - 1)

/* Valid range of Set_ID is 256 to 65535. 
 * Not checking 65535 condition since its satisfied 
 * by the fact that set_id is uint16
 */
#define BCM_INT_FT_TEMPLATE_SET_ID_VALID(set_id) (set_id >=256)

/* Set Id used when transmiting template sets */
#define FT_INT_TEMPLATE_XMIT_SET_ID 2

/* Default value of export interval. 100 ms */
#define BCM_INT_FT_DEF_EXPORT_INTERVAL_MSECS (100)

/* Default value of scan interval. 100 ms */
#define BCM_INT_FT_DEF_SCAN_INTERVAL_MSECS (100)

/* Default value of aging time. 1 minute in milliseconds */
#define BCM_INT_FT_DEF_AGING_INTERVAL_MSECS (60 * 1000)

/* Aging interval must be in steps of 1 sec */
#define BCM_INT_FT_AGING_INTERVAL_STEP_MSECS (1000)

/* Size of 1 data record
 * IPv4 SIP + IPv4 DIP + L4 Src Port + L4 Dst Port + Ip Protocol
 */
#define BCM_INT_FT_RECORD_LENGTH 25

/* Size of the template set header */
#define BCM_INT_FT_SET_HDR_LENGTH 4

/* Size of the IPFIX header */
#define BCM_INT_FT_IPFIX_MSG_HDR_LENGTH 16

/* Minimum length of the collecotor encap
 * UDP_HDR  =  8 bytes
 * IPv4_HDR = 20 bytes
 * ETH_HDR  = 14 bytes
 * L2_CRC   =  4 bytes
 * 8 + 20 + 14 + 4 = 46
 */
#define BCM_INT_FT_MIN_COLLECTOR_ENCAP_LENGTH 46

/* Minimum export length required */
#define BCM_INT_FT_MIN_EXPORT_LENGTH 128

/* Length of the L2 CRC field */
#define BCM_INT_FT_L2_CRC_LENGTH 4

/* Rx channel index used by FT EAPP */
#define BCM_INT_FT_EAPP_RX_CHANNEL 1

/* Maximum number of elephant profiles */
#define BCM_INT_FT_MAX_ELEPHANT_PROFILES 4

/* Elephant rate thresholds must be configured in steps of 8 kbits/sec*/
#define BCM_INT_FT_ELPH_RATE_THRESHOLD_STEP 8

/* Elephant profile Start Id */
#define BCM_INT_FT_ELPH_PROFILE_START_ID 1

/* Elephant profile END Id */
#define BCM_INT_FT_ELPH_PROFILE_END_ID \
    (BCM_INT_FT_ELPH_PROFILE_START_ID + BCM_INT_FT_MAX_ELEPHANT_PROFILES - 1)

/* Max number of actions that can be applied on a group */
#define BCM_INT_FT_MAX_GROUP_ACTIONS 5

/* Max number of params per action */
#define BCM_INT_FT_MAX_GROUP_ACTION_PARAMS 1

/*
 * Macro:
 *     _BCM_FT_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_FT_ALLOC(_ptr_,_ptype_,_size_,_descr_)                     \
            do {                                                         \
                if (NULL == (_ptr_)) {                                   \
                   (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_)); \
                }                                                        \
                if((_ptr_) != NULL) {                                    \
                    sal_memset((_ptr_), 0, (_size_));                    \
                }  else {                                                \
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, \
                              (BSL_META("FT Error: Allocation failure %s\n"), \
                               (_descr_)));                              \
                }                                                        \
            } while (0)

/* Max number of parts the actions is divided in H/w */
#define BCM_INT_FT_ACTION_MAX_PARTS 2

/* Group actions width, offset linked list */
typedef struct _bcm_int_ft_action_offset_s {
    /* Offset into the H/w buffer */
    uint16 offset[BCM_INT_FT_ACTION_MAX_PARTS];

    /* Width of the action*/
    uint8 width[BCM_INT_FT_ACTION_MAX_PARTS];

    /* Value that needs to be programmed */
    uint8 value[BCM_INT_FT_ACTION_MAX_PARTS];

} _bcm_int_ft_action_offset_t;

/* Flowtracker Internal action types */
typedef enum _bcm_ft_action_e {
    /* Change color of green packets */
    _bcmIntFtActionGpDropPrecedence = 0,

    /* Change color of yellow packets */
    _bcmIntFtActionYpDropPrecedence = 1,

    /* Change color of red packets */
    _bcmIntFtActionRpDropPrecedence = 2,

    /* Assign new Unicast cosq for Green packets */
    _bcmIntFtActionGpUcastCosQNew = 3,

    /* Assign new Unicast cosq for Yellow packets */
    _bcmIntFtActionYpUcastCosQNew = 4,

    /* Assign new Unicast cosq for Red packets */
    _bcmIntFtActionRpUcastCosQNew = 5,

    /* Assign new Multicast cosq for Green packets */
    _bcmIntFtActionGpMcastCosQNew = 6,

    /* Assign new Multicast cosq for Yellow packets */
    _bcmIntFtActionYpMcastCosQNew = 7,

    /* Assign new Multicast cosq for Red packets */
    _bcmIntFtActionRpMcastCosQNew = 8,

    /* Assign new Unicast & Multicast cosq for Green packets */
    _bcmIntFtActionGpCosQNew = 9,

    /* Assign new Unicast & Multicast cosq for Yellow packets */
    _bcmIntFtActionYpCosQNew = 10,

    /* Assign new Unicast & Multicast cosq for Red packets */
    _bcmIntFtActionRpCosQNew = 11,

    /* Change int_pri for Green packets */
    _bcmIntFtActionGpPrioIntNew = 12,

    /* Change int_pri for Yellow packets */
    _bcmIntFtActionYpPrioIntNew = 13,

    /* Change int_pri for Red packets */
    _bcmIntFtActionRpPrioIntNew = 14,

    /* Last value, not usable */
    _bcmIntFtActionCount
} _bcm_ft_action_t;

/* Actions info internal */
typedef struct _bcm_int_ft_action_info_s {
    /* Action element */
    _bcm_ft_action_t action;

    /* param0 */
    uint32 param0;

} _bcm_int_ft_action_info_t;

/* Group actions info */
typedef struct _bcm_int_ft_group_action_info_s {
    /* Action */
    bcm_flowtracker_group_action_t action;

    /* Number of params */
    int num_params;

    /* Params */
    uint32 params[BCM_INT_FT_MAX_GROUP_ACTION_PARAMS];
} _bcm_int_ft_group_action_info_t;

/* Per pipe information. */
typedef struct  _bcm_int_ft_pipe_info_s {
    /* Max flows */
    uint32 max_flows;

    /* Num ports in the pipe */
    uint16 num_ports;

    /* List of ports */
    uint16 ports[SHR_FT_MAX_PIPE_PORTS];

    /* Size of reserved pools */
    uint16 ctr_pool_size[SHR_FT_MAX_CTR_POOLS];

    /* Reserved Flex counter pools per pipe */
    uint8 ctr_pools[SHR_FT_MAX_CTR_POOLS];

    /* Number of reserved counter pools */
    uint8 num_ctr_pools;

    /* QoS profile mem ptr */
    soc_profile_mem_t *qos_profile_mem;
} _bcm_int_ft_pipe_info_t;

#define BCM_INT_FT_INFO_FLAGS_FLOW_START_TS (1 << 0)

/* Internal data structure to store
 * global flowtracker information.
 */
typedef struct _bcm_int_flowtracker_info_s {
    /* Elephant mode */
    uint8 elph_mode;

    /* flags, See BCM_INT_FT_INFO_FLAGS_XXX */
    uint32 flags;

    /* Max number of flow groups.
     * Got from soc property flowtracker_max_flow_groups
     */
    uint32 max_flow_groups;
    /* Max number of flows for entire switch.
     * Just a summation of max_flows_per_pipe for all pipes.
     */
    uint32 max_flows_total;
    /* The maximum MTU size that is possible for export
     * packets.
     * Got from soc property flowtracker_max_export_pkt_length
     */
    uint16 max_export_pkt_length;
    /* IPFIX observation domain Id */
    bcm_flowtracker_ipfix_observation_domain_t observation_domain_id;
    /* DMA Buffer used for messaging between SDK and
     * UKERNEL.
     */
    uint8 *dma_buffer;
    /* Length of the DMA buffer */
    int dma_buffer_len;
    /* Template ID pool */
    shr_idxres_list_handle_t template_pool;
    /* Collector ID pool */
    shr_idxres_list_handle_t collector_pool;
    /* Flow group ID pool */
    shr_idxres_list_handle_t flow_group_pool;
    /* Flow group ID pool */
    shr_idxres_list_handle_t elph_profile_pool;
    /* Number of pipes. */
    uint8 num_pipes; 
    /* Per pipe information. */
    _bcm_int_ft_pipe_info_t pipe_info[SHR_FT_MAX_PIPES];

    /* Enterprise Number */
    uint32 enterprise_number;

    /* Export interval (msecs) */
    uint32 export_interval;

    /* Scan interval (msecs) */
    uint32 scan_interval;

} _bcm_int_ft_info_t;


/* Internal data structure to store the information element data, used in
 * template set export
 */
typedef struct _bcm_int_ft_info_elem_s {
    /* Export element */
    shr_ft_template_info_elem_t element;

    /* Size of each element */
    uint8 data_size;

    /* Id corresponding to the export element, IANA assigned for
     * non-enterprise
     */
    uint16 id;

    /* 1 - Enterprise element, 0 - Non enterprise element */
    uint8 enterprise;
} _bcm_int_ft_export_element_t;

/* Internal data structure to store per export template
 * information.
 */
typedef struct _bcm_int_ft_export_template_info_s {
    /* Software ID of the template */
    bcm_flowtracker_export_template_t template_id;
    /* Set ID that is put into the IPFIX packet */
    uint16 set_id;
    /* Template set transmit interval */
    int interval_secs;
    /* Number of packets sent out initially */
    int initial_burst;
    /* Number of information elements */
    int num_export_elements;
    /* Export elements */
    _bcm_int_ft_export_element_t elements[SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];
} _bcm_int_ft_export_template_info_t;


/* Internal data structure to store per collector information
 */
typedef struct _bcm_int_ft_collector_info_s {
    /* Software ID of the collector */
    bcm_flowtracker_collector_t collector_id;
    /* Collector information structure */
    bcm_flowtracker_collector_info_t collector_info;
} _bcm_int_ft_collector_info_t;

/* Internal data structure to store per flow_group information
 */
typedef struct _bcm_int_ft_flow_group_info_s {
    /* Software ID of the flow_group */
    bcm_flowtracker_group_t flow_group_id;
    /* Aging interval in msecs */
    uint32 aging_interval_msecs;
    /* Max flows that can be learned on this group */
    uint32  flow_limit;
    /* Collector associated with the flow group */
    uint16 collector_id;
    /* Template associated with the flow group */
    uint16 template_id;

    /* Elephant profle Id */
    bcm_flowtracker_elephant_profile_t elph_profile_id;

    /* QoS profile Id */
    uint32 qos_profile_id;

    /* Number of actions on the group */
    uint16 num_actions;

    _bcm_int_ft_group_action_info_t *action_list;
} _bcm_int_ft_flow_group_info_t;


/* Internal data structure to store elephant profile info
 */
typedef struct _bcm_int_ft_elph_profile_info_s {
    /* Software Id of the profile */
    bcm_flowtracker_elephant_profile_t id;

    /* Profile information structure */
    bcm_flowtracker_elephant_profile_info_t profile;
} _bcm_int_ft_elph_profile_info_t;

/* Global data structures extern declarations */
extern _bcm_int_ft_info_t *ft_global_info[BCM_MAX_NUM_UNITS];

/* Utility functions */
extern void _bcm_esw_flowtracker_sw_dump(int unit);
extern int
_bcm_esw_flowtracker_ucast_cosq_resolve(int unit, uint32 cosq, int *hw_cosq);

extern int
_bcm_esw_flowtracker_mcast_cosq_resolve(int unit, uint32 cosq, int *hw_cosq);

/* Get the FT_INFO struct for the unit */
#define FT_INFO_GET(_u) ft_global_info[(_u)]


#endif /* INCLUDE_FLOWTRACKER */

extern int _bcm_esw_flowtracker_sync(int unit);


#endif  /* !_BCM_INT_FT_H_ */
