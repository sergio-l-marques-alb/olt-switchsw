
const int PFCDM_KEY_TEMPLATE_KEY_NUM_MAX = 10;
const int PFCDM_MAX_GROUP_NUM = 4;

/* pfcdm_field_type_e */
const int pfcdmFieldTypeByte = 0;
const int pfcdmFieldTypeHalfword = 1;
const int pfcdmFieldTypeWord = 2;

struct pfcdm_key_info_t {
    bcm_field_qualify_t qualify;
    uint32 mask;
    bcm_field_data_offset_base_t offset_base;
    uint16_t offset;
    int field_type;
};


/* pfcdm_key_e */
const int pfcdmKeyIPv4Sip = 0;
const int pfcdmKeyIPv4Dip = 1;
const int pfcdmKeyIPv4Protocol = 2;
const int pfcdmKeyIPv4SPort = 3;
const int pfcdmKeyIPv4DPort = 4;

struct pfcdm_key_template_t {
    uint8_t key_amount;
    int key_array[PFCDM_KEY_TEMPLATE_KEY_NUM_MAX]; 
};


const uint16_t PFCDM_KEYS_MAX_LEN = 80;
struct pfcdm_ipv4_5tuples_t{
    uint8_t protocol;
    uint32_t dip;
    uint32_t sip;
    uint16_t dport;
    uint16_t sport;
};

uint8_t keys[PFCDM_KEYS_MAX_LEN]; /* used for hash calculation */
uint16_t key_len;

struct pfcdm_group_table_entry_t {
    uint8_t key_template_id;
    uint16_t flow_age_cnt_cfg;
    uint32_t flow_count_limit;
    uint32_t flow_count_learn_limit;
    uint32_t flow_count_learn;
    uint32_t flow_count_static;
};

struct pfcdm_flow_action_t {
    uint8_t last_sw_drop_reason;
    /* uint8_t snoop_disable; */
};

const int pfcdm_pmf_qualify_max_num = 10;
const int pfcdm_pmf_action_max_num = 5;

struct pfcdm_pmf_group_action_list_t{
    uint8_t nof_actions;
    uint8_t action_pri_en; /* whether to configure priority or not */
    int action_info_pri;
    bcm_field_action_t action_types[pfcdm_pmf_action_max_num];
};

struct pfcdm_pmf_group_qualify_list_t{
    uint8_t nof_quals;
    bcm_field_qualify_t qual_types[pfcdm_pmf_qualify_max_num];
    bcm_field_input_types_t input_type[pfcdm_pmf_qualify_max_num];
    int input_arg[pfcdm_pmf_qualify_max_num];
    int offset[pfcdm_pmf_qualify_max_num];
};

const int PFCDF_SW_DROP_REASON_WORDS_LEN = 32;
struct pfcdm_flow_entry_t
{
    uint8_t flag;
    uint16_t aging_ticks;
    uint32_t stat_id;
    uint32_t flow_list_entry_id_head;
    uint8_t flow_state;
    uint16_t NW_flow_id;
    uint16_t group_id; 
    pfcdm_ipv4_5tuples_t key;
    uint32_t drop_packet_cnt;
    uint32_t drop_byte_cnt;
    uint32_t sw_drop_reason_bitmap[PFCDF_SW_DROP_REASON_WORDS_LEN];
    uint8_t sw_last_drop_reason;
};

/*
"0" indicates this entry is NOT valid;
"1" indicates static entry; 
"2" indicates for dynmical learning.
*/
const int PFCDM_FLOW_ENTRY_INVALID = 0;
const int PFCDM_FLOW_ENTRY_STATIC = 1;
const int PFCDM_FLOW_ENTRY_DYNMICAL = 2;

/*
0x0: No Congestion/Drop
0x1: Congestion/Drop start
0x2: Congestion/Drop on-going (optional)
0x3: Congestion/Drop end
*/
const int PFCDM_FLOW_STATE_DROP_NONE = 0;
const int PFCDM_FLOW_STATE_DROP_START = 1;
const int PFCDM_FLOW_STATE_DROP_ON_GOING = 2;
const int PFCDM_FLOW_STATE_DROP_END = 3;



/* 16k size */
const int PFCDM_FLOW_ROOT_TABLE_SIZE = 200; /* 16 * 1024; */
const int PFCDM_GLOBAL_FLOW_LIST_TABLE_SIZE = 50; /* 2 * 1024; */


struct pfcdm_flow_root_entry_t{
    uint16_t flow_count;
    uint32_t em_key;
    uint32_t flow_list_head;
    pfcdm_flow_entry_t flow_entry;
};

struct pfcdm_flow_list_entry_t {
    uint32_t flow_root_eid;

    bcm_field_entry_t tcam_entry_id;	
	
    uint32_t prev_entry_id;
    uint32_t next_entry_id;
    pfcdm_flow_entry_t flow_entry;	
};


const int PFCDM_FLOW_IN_EXEM3 = 1;
const int PFCDM_FLOW_IN_TCAM = 2;

const int PFCDM_INVALID_ENTRY_ID = -1;

const int PFCDM_VOQ_NUM = 64 * 1024; /* 64K */
const int PFCDM_VOS_CONGEST_MON_TABLE_SIZE = 512;

struct pfcdm_voq_congest_mon_entry_t{
    uint8_t valid;
    uint8_t congest_trigger_flag;
    uint16_t voq_out_port;
    uint16_t voq_priority;
    uint16_t voq_id;
    uint16_t congest_trigger_cnt; /* for debug */
    uint16_t age_cnt;
    uint32_t hw_entry_id;
};


const int local_host_cpu = 0;


const uint16_t PFCDM_HW_DO_NOTHING_TRAP_CODE = 9;

/* hardware trap code and software drop reason definition */
const uint8_t pfcdm_congest_sw_drop_reason = 1;

const uint16_t bcmRxTrapForwardingIpv4Ttl0_hw_code = 0x6c;
const uint8_t bcmRxTrapForwardingIpv4Ttl0_sw_drop_reason = 47; /* user defined */

const uint16_t bcmRxTrapUcLooseRpfFail_hw_code = 0xA6;
const uint8_t bcmRxTrapUcLooseRpfFail_sw_drop_reason = 12; /* user defined */

const uint16_t bcmRxTrapUnknowDest_hw_code = 0x0a;
const uint8_t bcmRxTrapUnknowDest_sw_drop_reason = 77; /* user defined */

const uint32_t drop_forward_val = 0x001FFFFF;


const int PFCDM_COUNTER_ENGINE_NUM = 1;
const int PFCDM_COUNTER_COMMAND = 9;
const int PFCDM_COUNTER_TYPE_ID = 0;
const int PFCDM_COUNTER_NUM_MAX = 250; /* 250 for reference code */
