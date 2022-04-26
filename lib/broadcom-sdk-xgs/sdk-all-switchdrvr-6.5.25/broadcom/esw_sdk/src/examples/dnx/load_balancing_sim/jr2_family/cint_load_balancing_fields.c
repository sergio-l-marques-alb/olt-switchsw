/*
 * $Id: cint_load_balancing_fields.c
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
    /*
     * The Virtual register ID that indicates whether the field is enabled in hashing.
     */
    uint32 virtual_register_enable;
    /*
     * The virtual register ID that indicates whether the filed is being hashed symmetrically if relevant.
     */
    uint32 virtual_register_symmetrical;
};

/*
 * MAX number of fields that can be available to a header.
 */
int MAX_NOF_FIELDS_OFFSETS = 3;
/*
 * List of supported header types.
 */
int LB_SIM_HEADER_MPLS = -1;
int LB_SIM_HEADER_ETH_INNER = 0;
int LB_SIM_HEADER_ETH_OUTER = 1;
int LB_SIM_HEADER_ETH_TAG_1 = 2;
int LB_SIM_HEADER_ETH_TAG_2 = 3;
int LB_SIM_HEADER_IPV4_INNER = 4;
int LB_SIM_HEADER_IPV4_OUTER = 5;
int LB_SIM_HEADER_IPV6 = 6;
int LB_SIM_HEADER_L4 = 7;
int LB_SIM_HEADER_GTP = 8;

LB_SIM_HEADER_MPLS=9;
int LB_SIM_NOF_HEADERS = 10;

field_offset_t headers_fields_offsets_info[10][3] = {
        {{0,48,48,0,1},  {96,16,-1,2,-1},{-1,-1,-1,-1,-1}}, /** ETH_INNER */
        {{0,48,48,3,4},  {96,16,-1,5,-1},{-1,-1,-1,-1,-1}}, /** ETH_OUTER */
        {{20,12,-1,6,-1},{32,16,-1,7,-1},{-1,-1,-1,-1,-1}}, /** ETH_TAG_1 */
        {{20,12,-1,8,-1},{32,16,-1,9,-1},{-1,-1,-1,-1,-1}},/** ETH_TAG_2 */
        {{96,32,128,10,11},{72,8,-1,12,-1},{-1,-1,-1,-1,-1}},  /** IPV4_INNER */
        {{96,32,128,13,14},{72,8,-1,15,-1},{-1,-1,-1,-1,-1}},  /** IPV4_OUTER */
        {{12,20,-1,16,-1},{48,8,-1,17,-1},{64,128,192,18,19}},  /** IPV6 */
        {{0,16,16,20,21},{-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1}},  /** L4 */
        {{32,32,-1,22,-1},{-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1}}, /** GTP */
        {{-1,-1,-1,23,-1},{-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1}}, /** MPLS */
};




