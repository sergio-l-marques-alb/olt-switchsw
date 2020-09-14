/*
 * $Id: cint_load_balancing_fields_ag.c
 */
/*
 * This is an auto generated file that is used by the load balancing simulator to receive the headers fields that participate in the hashing.
 */
/*
 * This structure hold a load balancing information of a field inside a header (e.g. the DIP field in an IPv4 header)
 */
struct field_offset_t
{
    /*
     * The field offset in bits (relative to the header start offset)
     */
    int offset;
    /*
     * The size of the fields in bits.
     */
    int size;
    /*
     * In case this field is configured as symmetrical with other part of the header, this offset in bits will point to the
     * Symmetrical part position in the header.
     */
    int symmetrical_offset;
};

/*
 * MAX number of fields that can be available to a header.
 */
int MAX_NOF_FIELDS_OFFSETS=3;
/*
 * List of supported header types.
 */
int LB_SIM_HEADER_MPLS = -1;
int LB_SIM_HEADER_ETH = 0;
int LB_SIM_HEADER_ETH_TAG_1 = 1;
int LB_SIM_HEADER_ETH_TAG_2 = 2;
int LB_SIM_HEADER_IPV4 = 3;
int LB_SIM_HEADER_IPV6 = 4;
int LB_SIM_HEADER_L4 = 5;
int LB_SIM_HEADER_GTP = 6;
int LB_SIM_NOF_HEADERS = 7;

int LB_SIM_HEADER_MPLS_FIELD_DISABLED = 0x1;
int LB_SIM_HEADER_ETH_FIELD_ADDRESS_DISABLED = 0x1;
int LB_SIM_HEADER_ETH_FIELD_ETHERTYPE_DISABLED = 0x2;
int LB_SIM_HEADER_IPV4_FIELD_IP_DISABLED = 0x1;
int LB_SIM_HEADER_IPV4_FIELD_PROTOCOL_DISABLED = 0x2;
int LB_SIM_HEADER_IPV6_FIELD_FLOW_LABEL_DISABLED = 0x1;
int LB_SIM_HEADER_IPV6_FIELD_NXT_HEADER_DISABLED = 0x2;
int LB_SIM_HEADER_IPV6_FIELD_IP_DISABLED = 0x4;
int LB_SIM_HEADER_L4_FIELD_PORT_DISABLED = 0x1;
int LB_SIM_HEADER_NON_SYMMETRICAL_HASHING = 0x8;

int LB_SIM_HEADER_ALL_ENABLED = 0;

field_offset_t headers_fields_offsets_info[7][3] = {
        {{0,48,48},{96,16,-1},{-1,-1,-1}},
        {{20,12,-1},{32,16,-1},{-1,-1,-1}},
        {{20,12,-1},{32,16,-1},{-1,-1,-1}},
        {{96,32,128},{72,8,-1},{-1,-1,-1}},
        {{12,20,-1},{48,8,-1},{64,128,192}},
        {{0,16,16},{-1,-1,-1},{-1,-1,-1}},
        {{32,32,-1},{-1,-1,-1},{-1,-1,-1}}
};




