/** \file kaps_verifier_parse_scenario.c
 *
 * Ctests for KAPS
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <shared/libxml/parser.h>
#include <shared/libxml/tree.h>

#include "kaps_verifier_xml_parse.h"
#include "kaps_verifier_parse.h"
#include "kaps_verifier.h"
#include "kaps_device_internal.h"
#include "kaps_ad.h"
#include "kaps_lpm_algo.h"
#include "kaps_key_internal.h"
#include "kaps_instruction_internal.h"
#include "kaps_it_mgr.h"
#include "kaps_db_internal.h"
#include "kaps_resource.h"
#include "kaps_algo_common.h"
#include "kaps_algo_hw.h"
#include "kaps_dma.h"

kaps_status rop_status = KAPS_OK;

/**
  @addtogroup XML_PARSE_APIS

  @section XML_FORMAT Format of XML Files

  The input XML file is a collection of databases specified with instructions.
  The keywords or identifiers are case sensitive. For user convenience, style sheets
  have been provided, which can be pointed to in the XML to render them in a more
  readable format in the browser. See the scenario examples for hints on using the
  style sheets.

  @subsection ACL_XML Specifying ACL Databases

  A single ACL database can be specified in the following XML schema:

  @code
        <!-- ACL database with ID 1 (This ID is presented to the control plane API when initializing the database). -->
        <!-- The ID must be unique within the specification -->
        <acl id='1'>
             <!-- Path to a valid input file that contains the datasets in KBP standard format. -->
             <!-- The entries from the input are used to populate the databases and determine -->
             <!-- the resources required -->
             <input>/foo/bar/acl.txt</input>

             <!-- The capacity of the database as expected by the user -->
             <capacity>100K</capacity>

             <!-- AD specification, id must be unique within the scenario -->
             <ad id='1'>
               <!-- width in bits -->
               <width>24</width>
               <!-- number of AD records. If specified, ACEs indirect to limited number of AD. -->
               <!-- If unspecified AD is one to one -->
               <capacity>100K</capacity>
             </ad>

             <!-- Required specification. An explicit key layout can be specified, -->
             <!-- or a single field with key_width must be specified. This specification -->
             <!-- is used to perform KPU manipulation as well as check for correctness. -->
             <!-- Internal field widths are in bits -->
             <!-- In addition each key field can take an attribute that describes the -->
             <!-- nature of the field, these are namely -->
             <!--   o type='tid' for Table ID -->
             <!--   o type='em' for Exact match -->
             <!--   o type='prefix' for prefixes -->
             <!--   o type='range' for range comparisons like ports -->
             <!--   o type='ternary' arbitrary bits/mask comparisons -->
             <!-- If no type is provided the field is considered ternary. -->
             <key>
               <SIP type='prefix'>32</SIP>
               <DIP type='prefix'>32</DIP>
               <SPORT type='range'>16</SPORT>
               <DPORT type='range'>16</DPORT>
               <PROTO type='ternary'>8</PROTO>
               <FLAGS  type='ternary'>16</FLAGS>
             </key>

             <!-- Required if SMT is enabled, defines the thread to which the database belongs -->
             <!-- If SMT is disabled, this attribute is ignored -->
             <!-- Accepted values are device specific, should be an integer value typically, 0, 1 -->
             <thread>0</thread>
       </acl>

  @endcode

  ______

  @subsection LPM_XML Specifying an LPM Database

  A single LPM database can be specified with the following XML schema:

  @code
      <!-- LPM database with ID 10 (This ID is presented to the control plane API when initializing the database). -->
      <!-- The ID must be unique within the specification -->
      <lpm id='10'>
             <!-- Path to a valid input file that contains the datasets in KBP standard format. -->
             <!-- The entries from the input are used to populate the databases and determine -->
             <!-- the resources required -->
             <input>/foo/prefixes.txt</input>

             <!-- The capacity of the database as expected by the user -->
             <capacity>100K</capacity>

             <!-- AD specification, id must be unique -->
             <ad id='10'>
                 <!-- width in bits -->
                 <width>24</width>
                 <!-- number of AD records. If specified, ACEs indirect to limited number of AD. -->
                 <!-- If unspecified AD is one to one -->
                 <capacity>100K</capacity>
             </ad>

             <!-- HitBit Database Specification  -->
             <hb id='1'>
                  <!-- Capacity of HitBit database -->
                  <capacity>100k</capacity>
                  <!-- Time Interval -->
                  <age_count>100</age_count>
             </hb>

             <!-- Required specification. An explicit key layout can be specified, -->
             <!-- or a single field with key_width must be specified. This specification -->
             <!-- is used to perform KPU manipulation as well as check for correctness. -->
             <!-- Internal field widths are in bits -->
             <!-- In addition each key field can take an attribute that describes the -->
             <!-- nature of the field, these are namely -->
             <!--   o type='tid' for Table ID -->
             <!--   o type='em' for Exact match -->
             <!--   o type='prefix' for prefixes -->
             <!--   o type='range' for range comparisons like ports -->
             <!-- Ternary is not an acceptable type for LPM databases -->
             <key>
                 <TABLE_ID type='em'>8</TABLE_ID>
                 <VRF type='em'>16</VRF>
                 <SIP type='prefix'>32</SIP>
             </key>

             <!-- If any default entries need to specify -->
             <!-- valid Priorities are 0,1,2,3 -->
             <!-- only prefix is expected to be in decimal value with length -->
             <kaps_default_entries priority='0'>
                 <prefix>00 0000 0/8</prefix>
                 <prefix>00 0001 0.10/16</prefix>
                 <prefix>00 0002 0.233/16</prefix>
                 <prefix>00 0003 0.210.245/23</prefix>
             </kaps_default_entries>

             <!-- Required if SMT is enabled, defines the thread to which the database belongs -->
             <!-- If SMT is disabled, this attribute is ignored -->
             <!-- Accepted values are device specific, should be an integer value typically, 0, 1 -->
             <thread>1</thread>
        </lpm>
  @endcode

  ______

  @subsection EM_XML Specifying an Exact-Match Database

  A single EM database can be specified with the following XML schema:

  @code
        <!-- Exact match database with ID 3 (This ID is presented to the control plane API when initializing the database). -->
        <!-- The ID must be unique within the specification -->
        <em id='3'>
            <!-- Path to a valid input file that contains the datasets in KBP standard format. -->
            <!-- The entries from the input are used to populate the databases and determine -->
            <!-- the resources required -->
            <input>/foo/bar/em.txt</input>

            <!-- The capacity of the database as expected by the user -->
            <capacity>100K</capacity>

            <!-- AD specification, id must be unique -->
            <ad id='3'>
                <!-- width in bits -->
                <width>24</width>
                <!-- number of AD records. If specified, ACEs indirect to limited number of AD. -->
                <!-- If unspecified AD is one to one -->
                <capacity>50K</capacity>
            </ad>

             <!-- Required specification. An explicit key layout can be specified, -->
             <!-- or a single field with key_width must be specified. This specification -->
             <!-- is used to perform KPU manipulation as well as check for correctness. -->
             <!-- Internal field widths are in bits -->
             <!-- In addition each key field can take an attribute that describes the -->
             <!-- nature of the field, these are namely -->
             <!--   o type='tid' for Table ID -->
             <!--   o type='em' for Exact match -->
             <!-- Prefix, range, ternary types are not valid for exact match databases -->
             <key>
                 <SA type='em'>48</SA>
             </key>

             <!-- Required if SMT is enabled, defines the thread to which the database belongs -->
             <!-- If SMT is disabled, this attribute is ignored -->
             <!-- Accepted values are device specific, should be an integer value typically, 0, 1 -->
             <thread>0</thread>
       </em>

  @endcode
  ______

  @subsection TABLES_XML Specifying Tables Within Databases

  Tables within databases can be expressed using the following format. Any number
  of tables can be specified within a database, and is supported for ACL/LPM and
  EM databases. When tables are used there is a requirement for a key field with
  type 'tid' that is of the same width and at the same offset in the keys of all the tables.

  @code

    <acl id='0'>
      <!-- Global database arguments for capacity. The database also -->
      <!-- forms the first implicit table. All the arguments and -->
      <!-- constraints defined for regular databases apply as before -->
      <capacity>128K</capacity>
      <input>/path/to/my/input/tbl_001.txt</input>

      <!-- AD specification, id must be unique, the -->
      <!-- AD is shared among all tables -->
      <ad id='1'>
        <!-- width in bits -->
        <width>24</width>
        <!-- number of AD records. If specified, ACEs indirect to limited number of AD. -->
        <!-- If unspecified AD is one to one -->
        <capacity>128K</capacity>
      </ad>

      <!-- One field with type 'tid' is required in the key -->
      <key>
        <TID3 type='tid'>16</TID3>
        <RANGE6 type='range'>16</RANGE6>
        <RANGE7 type='range'>16</RANGE7>
        <EM2 type='em'>16</EM2>
      </key>

      <!-- The second table specification within the database -->
      <!-- The table ID must be unique within the database scope -->
      <table id = '0'>
        <input>/path/to/my/input/tbl_002.txt</input>

        <!-- One field with type 'tid' is required in the key -->
        <!-- It should be at the same offset for all tables and of same width -->
        <key>
          <TID0 type='tid'>16</TID0>
          <RANGE1 type='range'>16</RANGE1>
          <RANGE0 type='range'>16</RANGE0>
          <PREFIX1 type='prefix'>32</PREFIX1>
        </key>
      </table>

      <!-- The third table specification within the database -->
      <!-- The table ID must be unique within the database scope -->
      <table id = '1'>
        <input>/path/to/my/input/tbl_003.txt</input>
        <key>
          <TID1 type='tid'>16</TID1>
          <RANGE3 type='range'>16</RANGE3>
          <RANGE2 type='range'>16</RANGE2>
          <TERNARY1 type='ternary'>56</TERNARY1>
        </key>
      </table>
    </acl>

  @endcode
______

  @subsection CLONE_XML Specifying Multi Access Databases

  In many situations specifically related to forwarding, the same database may be looked up with different keys in
  the same/different cycle. A classic example is forwarding lookup and reverse-path-forwarding lookup. They share the
  same database but use different keys for lookup. The specification allows for describing such a use.

  @code

  <acl id='0'>
      <!-- Global database arguments for capacity. -->
      <!-- All the arguments and constraints defined for regular databases apply as before -->
      <capacity>128K</capacity>
      <input>/path/to/my/input/tbl_001.txt</input>

      <!-- AD specification, id must be unique, the -->
      <!-- AD is shared among all tables/clones -->
      <ad id='1'>
        <!-- width in bits -->
        <width>24</width>
        <!-- number of AD records. If specified, ACEs indirect to limited number of AD. -->
        <!-- If unspecified AD is one to one -->
        <capacity>128K</capacity>
      </ad>

      <!-- The main database/table key specification -->
      <key>
        <VRF type='em'>16</VRF>
        <DIP type='prefix'>32</DIP>
      </key>

      <!-- Multi access database specification. The same database/table -->
      <!-- can be accessed with a different view -->
      <!-- The ID is expected to be unique within database -->
      <!-- There are no separate input or other specification for a clone -->
      <!-- Only a different view/key can be specified -->
      <clone id='2'>
        <key>
          <VRF type='em'>16</VRF>
          <SIP type='prefix'>32</SIP>
        </key>
      </clone>
  </acl>

  @endcode

______

  @subsection COMBINED_XML Combining Databases into Instructions

  The following XML syntax can be used to combine various databases, tables
  multi access databases into instructions.

  @code
    <!-- instruction with ID 0. The ID has no meaning other than identifying the search in the schemas -->
    <instruction id='0'>
      <!-- Required master search key specification. Based on the master key -->
      <!-- the KPUs are programmed for the individual databases being searched -->
      <!-- The master key layout is as it appears in the context buffer -->
      <key>
        <VRF type='em'>16</VRF>
        <SIP type='prefix'>32</SIP>
        <DIP type='prefix'>32</DIP>
        <SPORT type='range'>16</SPORT>
        <DPORT type='range'>16</DPORT>
        <PROTO type='ternary'>8</PROTO>
        <FLAGS type='ternary'>16</FLAGS>
        <SA type='em'>48</SA>
      </key>

      <!-- various databases being searched, the order determines -->
      <!-- the result packing order. The first database specified -->
      <!-- is placed in result slot zero and so on -->
      <acl id='10' result='0'/>           <!-- search ACL with database id 10, result in slot 0 -->
      <acl id='20' table='1' result='2'/> <!-- search ACL with database id 20, and table 1 in it, result in slot 2 -->
      <lpm id='1' clone='2 result='1'/>  <!-- search LPM with database id 1, and search its multi access view with id 2, result in slot 1 -->

      <!-- Optional specification of LTR register number. The tool -->
      <!-- automatically determines the LTR number to use, as certain -->
      <!-- features require specific assignment of LTRs. This specification -->
      <!-- can be used as an override -->
      <ltr>1</ltr>

      <!-- Required if SMT is enabled, defines the thread to which the instruction belongs -->
      <!-- If SMT is disabled, this attribute is ignored -->
      <!-- Accepted values are device specific, should be an integer value typically, 0, 1 -->
      <thread>0</thread>
    </instruction>

  @endcode

  ______

  @subsection XML_CONSTRAINTS Database Specification Keywords and Constraints

  In addition to the basic specification of databases and instructions above, the following
  specific attributes are supported in the instructions and/or databases listed below. These
  attributes are optional, and should not be specified except for advanced overrides.

  | Keyword | Values | Details |
  | :---: | :---: | :--- |
  | kaps_prop_algorithmic   | 0,1,2,3 | For databases only. Zero corresponds to massively parallel, For LPM and EM databases 1,2,3 mean algorithmic. For ACL databases 1, 2 mean DBA based power control, 3 corresponds to SRAM based ACLs|
  | thread | 0,1 | When SMT is enabled corresponds to thread 0 or 1. Valid for both databases and instructions |
  | kaps_prop_index_range | min='0' max='10905190' | For databases only. Specify the min and max index range. <kaps_prop_index_range min='0' max='10905190'/> |
  | kaps_prop_description | string | A user visible description of name for database only. |
  | kaps_db_priority | integer | The default Priority value for all the entries in database |
  | kaps_hw_resource_dba | integer | The amount of DBA to use. Specified as number of array blocks |
  | kaps_hw_resource_uda | integer | The UDA in megabits |
  | kaps_hw_resource_range_units | integer | The number of range units to assign |

  ____

  Databases with searches are combined together and specified in a single file as a scenario. They are wrapped
  with @verbatim <scenario></scenario> @endverbatim tags and can be fed to tools to verify correctness, estimate
  whether they can fit in the device, and compute various hardware resources and configurations.

  @section XML_SERDES_FORMAT Format of serdes configuration file

  The following XML format configuration file should be used to specify the serdes configuration.

  @code
  <!-- XML must start with a scenario with options name which describes the configuration -->
  <scenario name ="serdes init config for : OP single_port, no_smt, speed=10.3GHz, burst=8 and metaframe=128">
    <!-- Start interface specification details -->
    <interface>
      <smt>1</smt>                 <!-- Enable/Disable SMT 0=> NO_SMT (single bank), 1=> SMT (dual bank) -->
      <dual_port>1</dual_port>     <!-- 0=> Single port, 1=> Dual port. Default if unspecified is single port -->
      <speed>3</speed>             <!-- Serdes speed  0=> 12.5G, 1=> 3.125G, 2=> 6.25G, 3=> 10.3G, 4=> 1.25G
                                        5 ==> 10G SerDes, 6=> 15G, 7=> 25.7G, 8=> 28G SerDes. -->
      <burst>8</burst>             <!-- Burst short either 8 or 16 -->
      <meta_len>128</meta_len>     <!-- Meta Frame length minimum 128, maximum 8K -->
      <port id='0'>                <!-- Start lane configuration for first port, must be present -->
        <start_lane>0</start_lane> <!-- Start lane number -->
        <num_lanes>12</num_lanes>  <!-- Number of contiguous lanes -->
      </port>
      <port id='1'>                <!-- Must be specified if dual port is selected -->
        <start_lane>0</start_lane> <!-- Start lane number must not overlap with above port lanes -->
        <num_lanes>12</num_lanes>  <!-- Number of contiguous lanes -->
      </port>
      <reverse>1</reverse>         <!-- Reverse the lanes if set to 1 -->
    </interface>
  </scenario>
  @endcode

  The above configuration file should be specified with the -f option to generate the serdes init sequence.

 @}

  @addtogroup EVAL_FORMATS Input File Formats
  @{

  The input datasets that are specified to the user scenarios require
  the Broadcom defined format. The file format is described below and samples
  are provided as part of the package for reference. The first line of the file must
  start with \e +KBP-STANDARD-FORMAT. The next valid line specifies the key format
  for the rest of the file. Each key element has three
  attributes. Any lines prefixed with the \e # character
  are ignored as comments. Comments must start at the
  beginning of the line and the whole line is ignored.

  <ul>
    <li> Name of the field
    <li> Width of the field in bits
    <li> Type of the field. The types can be
    <ul>
      <li> \e tid. Exact match table ID. Specified in hexadecimal format.
      <li> \e prefix. IP address specified as address/mask in hexadecimal format.
      <li> \e em. Exact-match field specified in hexadecimal format.
      <li> \e range. Port ranges specified in hexadecimal format as \e lo-hi, where lo and hi are inclusive.
      <li> \e ternary. Arbitrary bits and mask expressed as nibble values separated by spaces; for example, 11** 0*1*.
    </ul>
  </ul>

  Examples of IPv4 and IPv6 formats are shown below.

  @code

  +KBP-STANDARD-FORMAT
  (SIP,32,prefix) (DIP,32,prefix) (SPORT,16,range) (DPORT,16,range) (PROTOCOL,8,em) (FLAGS,16,ternary)
  # My IPv4 ACL file
  bb.df.1a.7b/00.00.00.00 49.40.14.82/00.00.00.00 0000-ffff 05f5-05f5 06 **** **0* **** ****
  bb.df.1a.2c/00.00.00.00 49.40.14.82/00.00.00.00 0000-ffff 39a1-39a1 06 **** **0* **** ****

  @endcode

  @code

  +KBP-STANDARD-FORMAT
  (SIP,128,prefix) (DIP,128,prefix) (SPORT,16,range) (DPORT,16,range) (PROTOCOL,8,em)
  # My IPv6 file
  e0.9a.df.30.00.00.00.00.00.00.00.00.00.00.00.00/00.00.00.00.7f.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff d8.da.3a.ca.80.00.00.00.00.00.00.00.00.00.00.00/00.00.00.00.7f.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff 0000-ffff 05f1-05f1 **
  e0.9d.fe.39.80.00.00.00.00.00.00.00.00.00.00.00/00.00.00.00.7f.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff d8.da.3a.da.00.00.00.00.00.00.00.00.00.00.00.00/00.00.00.00.7f.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff 0000-ffff 0598-0598 02
  e1.fd.f8.34.00.00.00.00.00.00.00.00.00.00.00.00/00.00.00.00.7f.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff d8.da.3b.99.00.00.00.00.00.00.00.00.00.00.00.00/00.00.00.00.7f.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff 0000-ffff 06b5-06b5 01
  e8.b9.8a.e7.80.00.00.00.00.00.00.00.00.00.00.00/00.00.00.00.7f.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff d8.da.7a.7b.00.00.00.00.00.00.00.00.00.00.00.00/00.00.00.00.7f.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff.ff 0000-ffff 7854-7854 01

  @endcode

  An example of an LPM input file is specified below.

  @code

  +KBP-STANDARD-FORMAT
  (PREFIX_VAL,32,prefix)
  02.00.00.00/00.00.ff.ff
  02.01.00.00/00.00.07.ff
  02.01.18.00/00.00.00.ff
  03.00.00.00/00.ff.ff.ff
  03.33.5c.00/00.00.01.ff
  @endcode

  An example of an specifying the ad data in the input file is specified below. Note: Always ad data is expected to be at the end.

  @code

  +KBP-STANDARD-FORMAT
  (PREFIX_VAL,32,prefix) (user_ad, 48, ad)
  02.00.00.00/00.00.ff.ff abcdabcdabcd
  02.01.00.00/00.00.07.ff abcdabcdabcd
  02.01.18.00/00.00.00.ff abcdabcdabcd
  03.00.00.00/00.ff.ff.ff abcdabcdabcd
  03.33.5c.00/00.00.01.ff abcdabcdabcd
  @endcode

  @}

*/

struct device_properties
{
    char prop_name[256];
    uint32_t property;
};

struct db_properties
{
    char prop_name[256];
    uint32_t property;
};

struct hw_resource_properties
{
    char prop_name[256];
    uint32_t property;
};

struct acl_db_advanced_properties
{
    char prop_name[256];
    uint32_t property;
};

struct db_properties db_prop[] = {
    {"kaps_prop_use_mcor", KAPS_PROP_USE_MCOR},
    {"kaps_prop_mcor_value", KAPS_PROP_MCOR_VALUE},
    {"kaps_prop_algorithmic", KAPS_PROP_ALGORITHMIC},
    {"kaps_prop_index_range", KAPS_PROP_INDEX_RANGE},
    {"kaps_prop_index_callback", KAPS_PROP_INDEX_CALLBACK},
    {"kaps_prop_min_priority", KAPS_PROP_MIN_PRIORITY},
    {"kaps_prop_max_priority", KAPS_PROP_MAX_PRIORITY},
    {"kaps_prop_description", KAPS_PROP_DESCRIPTION},
    {"kaps_prop_cascade_device", KAPS_PROP_CASCADE_DEVICE},
    {"kaps_prop_reduced_index_callbacks", KAPS_PROP_REDUCED_INDEX_CALLBACKS},
    {"kaps_prop_enable_db_compaction", KAPS_PROP_ENABLE_DB_COMPACTION},
    {"kaps_prop_max_capacity", KAPS_PROP_MAX_CAPACITY},
    {"kaps_prop_pair_db", KAPS_PROP_PAIR_WITH_DB},
    {"kaps_prop_defer_deletes", KAPS_PROP_DEFER_DELETES},
    {"kaps_prop_num_bbs", KAPS_PROP_NUM_BBS},
    {"kaps_prop_scale_up_capacity", KAPS_PROP_SCALE_UP_CAPACITY},
    {"kaps_prop_enable_counters", KAPS_PROP_ENABLE_COUNTERS},
    {"kaps_prop_counter_type", KAPS_PROP_COUNTER_TYPE},
    {"kaps_prop_counter_cycle", KAPS_PROP_COUNTER_CYCLE},
    {"kaps_prop_xl_db", KAPS_PROP_XL_DB},
    {"kaps_prop_locality", KAPS_PROP_LOCALITY},
    {"kaps_prop_replicate_db", KAPS_PROP_REPLICATE_DB},
    {"kaps_prop_mc_db", KAPS_PROP_MC_DB},
    {"kaps_prop_enable_dynamic_allocation", KAPS_PROP_ENABLE_DYNAMIC_ALLOCATION},
    {"kaps_prop_utilise_uda_holes", KAPS_PROP_UTILISE_UDA_HOLES}
};

struct device_properties device_prop[] = {
    {"kaps_device_adv_uda_write", KAPS_DEVICE_ADV_UDA_WRITE},
    {"kaps_device_prop_adv_uda_write", KAPS_DEVICE_PROP_ADV_UDA_WRITE},
    {"kaps_device_pre_clear_abs", KAPS_DEVICE_PRE_CLEAR_ABS},
    {"kaps_device_prop_inst_latency", KAPS_DEVICE_PROP_INST_LATENCY},
    {"kaps_device_prop_lpt_mode", KAPS_DEVICE_PROP_LPT_MODE},
    {"kaps_device_prop_counter_compression", KAPS_DEVICE_PROP_COUNTER_COMPRESSION},
    {"kaps_device_prop_null_record_size", KAPS_DEVICE_PROP_TAP_NULL_REC_SIZE},
    {"kaps_device_prop_ingress_record_size", KAPS_DEVICE_PROP_TAP_INGRESS_REC_SIZE},
    {"kaps_device_prop_egress_record_size", KAPS_DEVICE_PROP_TAP_EGRESS_REC_SIZE},
    {"kaps_device_prop_ingress_opcode_ext_start_location", KAPS_DEVICE_PROP_TAP_INGRESS_OPCODE_EXT_START_LOC},
    {"kaps_device_prop_egress_opcode_ext_start_location", KAPS_DEVICE_PROP_TAP_EGRESS_OPCODE_EXT_START_LOC},
    {"kaps_device_prop_ingress_opcode_ext_len", KAPS_DEVICE_PROP_TAP_INGRESS_OPCODE_EXT_LEN},
    {"kaps_device_prop_egress_opcode_ext_len", KAPS_DEVICE_PROP_TAP_EGRESS_OPCODE_EXT_LEN}
};

struct hw_resource_properties hw_rxc_prop[] = {
    {"kaps_hw_resource_dba", KAPS_HW_RESOURCE_DBA},
    {"kaps_hw_resource_uda", KAPS_HW_RESOURCE_UDA},
    {"kaps_hw_resource_range_units", KAPS_HW_RESOURCE_RANGE_UNITS}
};

struct stats_database_params
{
    char *arr_ptr[4];
    int32_t num_ptr;
    char *attr[8];
    int num_attr;
    int attribute[256];
    int nums_of_cols;
};

struct stats_record_params
{
    char *record_field[12];
    int32_t length[12];
};

#define STATRECLIST_TO_ENTRY(ptr) CONTAINER_OF((ptr), struct kaps_stats_record, node)

#define NUM_DB_PROP     (sizeof(db_prop) / sizeof(struct db_properties))
#define NUM_DEVICE_PROP (sizeof(device_prop) / sizeof(struct device_properties))
#define NUM_HW_RXC_PROP (sizeof(hw_rxc_prop) / sizeof(struct hw_resource_properties))

static void
print_error(
    const char *format,
    ...)
{
    va_list ap;
    kaps_printf("\033[1;31mERROR: \033[0m ");
    va_start(ap, format);
    kaps_vprintf(format, ap);
    va_end(ap);
}

static void
print_key_verification_error(
    struct kaps_instruction *instruction,
    struct xml_parse_info *info,
    kaps_status error_code,
    char *error_field_name)
{
    switch (error_code)
    {
        case KAPS_KEY_FIELD_MISSING:
            print_error("could not find key field \"%s\" specified in database "
                        "at line %d in the search key\n", error_field_name, info->line_no);
            break;
        case KAPS_KEY_GRAN_ERROR:
            print_error("For database at line %d keys can be extracted "
                        "from search key only on byte boundaries\n", info->line_no);
            break;
        default:
            break;
    }
}

static kaps_status
free_xml_key(
    struct kaps_parse_key *xml_key)
{
    struct kaps_parse_key *del_field, *cur_field;

    cur_field = xml_key;
    while (cur_field)
    {
        del_field = cur_field;
        cur_field = cur_field->next;
        kaps_sysfree(del_field);
    }

    return KAPS_OK;
}

static int32_t
parse_int(
    const char *fname,
    uint32_t line,
    xmlChar * content)
{
    char *end;
    int32_t val = sal_strtol((const char *) content, &end, 10);

    if (end == (const char *) content)
    {
        print_error("%s:%d Error converting %s to int\n", fname, line, content);
        return -1;
    }
    if (end == NULL)
        return val;

    if (*end == 'k' || *end == 'K')
        val *= 1024;
    else if (*end == 'm' || *end == 'M')
        val *= 1048576;
    else if (*end == 'g' || *end == 'G')
        val *= 1073741824;      /* overflow */
    return val;
}

static kaps_status
expand_inputs(
    const char *fname,
    struct xml_parse_info *info,
    const char *path)
{
    uint32_t max_inputs = 10, ninputs = 0;
    char **input;
    char *running, *head, *token;
    int length;

    input = kaps_sysmalloc(max_inputs * sizeof(char *));
    if (!input)
    {
        kaps_sysfree(input);
        return KAPS_OUT_OF_MEMORY;
    }

    length = sal_strnlen(path, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1;
    head = running = kaps_sysmalloc(length);
    kaps_memcpy(head, path, length);

    for (token = kaps_strsep(&running, " "); token; token = kaps_strsep(&running, " "))
    {
        while (sal_isspace(*token))
            token++;
        if (*token == '\0')
            continue;

        if (ninputs == max_inputs)
        {
            char **res = realloc(input, max_inputs * 2 * sizeof(char *));

            if (res == NULL)
            {
                int32_t i;

                for (i = 0; i < ninputs; i++)
                {
                    kaps_sysfree(input[i]);
                }
                kaps_sysfree(input);
                kaps_sysfree(head);
                return KAPS_OUT_OF_MEMORY;
            }
            input = res;
            max_inputs *= 2;
        }

        length = sal_strnlen(token, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1;
        input[ninputs] = kaps_sysmalloc(length);
        kaps_memcpy(input[ninputs], token, length);

        ninputs++;
    }

    if (ninputs == 0)
    {
        print_error("%s:%d Failed to find/expand inputs for database\n", fname, info->line_no);
        kaps_sysfree(input);
        kaps_sysfree(head);
        return KAPS_PARSE_ERROR;
    }

    kaps_sysfree(head);
    info->num_inputs = ninputs;
    info->inputs = input;
    return KAPS_OK;
}

static kaps_status
parse_key(
    const char *fname,
    xmlNode * node,
    struct kaps_device *device,
    struct kaps_parse_key **pkey,
    struct kaps_key **key,
    uint16_t * width_1)
{
    struct kaps_key *new_key;
    uint16_t field_width;
    kaps_status status;
    struct kaps_parse_key *tail = NULL;

    *width_1 = 0;
    status = kaps_key_init(device, &new_key);
    if (status != KAPS_OK)
    {
        print_error("%s:%d Error: %s\n", fname, node->line, kaps_get_status_string(status));
        return KAPS_PARSE_ERROR;
    }

    for (node = node->children; node != NULL; node = node->next)
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            xmlChar *content = xmlNodeGetContent(node);
            xmlChar *ktype;
            struct kaps_parse_key *tmp;
            enum kaps_key_field_type type = KAPS_KEY_FIELD_TERNARY;
            int16_t field_offset = -1;

            if (!content)
            {
                print_error("%s:%d Expected bit offset for key field\n", fname, node->line);
                return KAPS_PARSE_ERROR;
            }
            ktype = xmlGetProp(node, BAD_CAST "type");
            if (ktype != NULL)
            {
                if (xmlStrcasecmp(ktype, BAD_CAST "ternary") == 0)
                    type = KAPS_KEY_FIELD_TERNARY;
                else if (xmlStrcasecmp(ktype, BAD_CAST "prefix") == 0)
                    type = KAPS_KEY_FIELD_PREFIX;
                else if (xmlStrcasecmp(ktype, BAD_CAST "em") == 0)
                    type = KAPS_KEY_FIELD_EM;
                else if (xmlStrcasecmp(ktype, BAD_CAST "range") == 0)
                    type = KAPS_KEY_FIELD_RANGE;
                else if (xmlStrcasecmp(ktype, BAD_CAST "tid") == 0)
                    type = KAPS_KEY_FIELD_TABLE_ID;
                else if (xmlStrcasecmp(ktype, BAD_CAST "hole") == 0)
                    type = KAPS_KEY_FIELD_HOLE;
                else if (xmlStrcasecmp(ktype, BAD_CAST "dummyfill") == 0)
                    type = KAPS_KEY_FIELD_DUMMY_FILL;
                else
                {
                    print_error("%s:%d unexpected type %s for key field %s\n", fname, node->line, ktype, node->name);
                    return KAPS_PARSE_ERROR;
                }
                xmlFree(ktype);
            }
            field_width = parse_int(fname, node->line, content);
            xmlFree(content);

            ktype = xmlGetProp(node, BAD_CAST "offset");
            if (ktype != NULL)
            {
                field_offset = parse_int(fname, node->line, ktype);
                xmlFree(ktype);
            }
            tmp =
                kaps_syscalloc(1,
                               sizeof(struct kaps_parse_key) + sal_strnlen((char *) node->name,
                                                                           SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)
                               + 1);
            if (tmp == NULL)
            {
                print_error("%s:%d Ran out of memory allocating key field\n", fname, node->line);
                return KAPS_PARSE_ERROR;
            }

            tmp->fname = tmp->buf;
            sal_strncpy(tmp->fname, (char *) node->name,
                        sal_strnlen((char *) node->name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
            tmp->width = field_width;
            tmp->offset = field_offset;
            tmp->type = type;

            if (tail == NULL)
            {
                *pkey = tmp;
            }
            else
            {
                tail->next = tmp;
            }
            tail = tmp;
            *width_1 = *width_1 + field_width;
            if (field_offset != -1)
            {
                tmp->overlay_field = 1;
                status = kaps_key_overlay_field(new_key, (char *) node->name, field_width, type, field_offset);
            }
            else
                status = kaps_key_add_field(new_key, (char *) node->name, field_width, type);
            if (status != KAPS_OK)
            {
                print_error("%s:%d Key field creation failed. %s\n", fname, node->line, kaps_get_status_string(status));
                return KAPS_PARSE_ERROR;
            }
        }
    }

    *key = new_key;
    return KAPS_OK;
}

static kaps_status
alloc_bc_databases(
    xmlNode * node,
    struct xml_parse_info *info,
    struct kaps_device **bc_devices,
    uint32_t num_devices)
{
    kaps_status status;
    uint32_t line_no = node->line;

    for (node = node->children; node != NULL; node = node->next)
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            xmlChar *content = xmlNodeGetContent(node);
            if (content && xmlStrcasecmp(node->name, BAD_CAST "device_map") == 0)
            {
                char *buf = (char *) content;
                char *token;
                uint32_t count = 0;

                info->bc_dbs = kaps_syscalloc(num_devices, sizeof(struct kaps_db *));
                if (info->bc_dbs == NULL)
                    return KAPS_OUT_OF_MEMORY;
                count = 0;
                for (token = kaps_strsep(&buf, ","); token; token = kaps_strsep(&buf, ","))
                {
                    uint32_t dev_no = 0;

                    while (sal_isspace(*token))
                        token++;
                    if (*token == '\0')
                        continue;

                    dev_no = sal_atoi(token);
                    kaps_sassert(dev_no < num_devices);

                    if (count == 0)
                    {
                        info->bc_dbs[dev_no] = info->db.db;
                        count++;
                        continue;
                    }

                    status = kaps_db_create_broadcast(bc_devices[dev_no], info->db.db, &info->bc_dbs[dev_no]);
                    if (status != KAPS_OK)
                    {
                        print_error("Error :%d, While creating the broadcast db: %s\n", line_no,
                                    kaps_get_status_string(status));
                        return status;
                    }
                }
            }
            xmlFree(content);
        }
    }
    return KAPS_OK;
}

static kaps_status
clone_bc_databases(
    xmlNode * node,
    struct xml_parse_info *parent,
    struct xml_parse_info *info,
    struct kaps_device **bc_devices,
    uint32_t num_devices)
{
    kaps_status status;
    uint32_t line_no = node->line;

    for (node = node->children; node != NULL; node = node->next)
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            xmlChar *content = xmlNodeGetContent(node);
            if (content && xmlStrcasecmp(node->name, BAD_CAST "device_map") == 0)
            {
                char *buf = (char *) content;
                char *token;
                uint32_t count = 0;

                info->bc_dbs = kaps_syscalloc(num_devices, sizeof(struct kaps_db *));
                if (info->bc_dbs == NULL)
                    return KAPS_OUT_OF_MEMORY;
                count = 0;
                for (token = kaps_strsep(&buf, ","); token; token = kaps_strsep(&buf, ","))
                {
                    uint32_t dev_no = 0;

                    while (sal_isspace(*token))
                        token++;
                    if (*token == '\0')
                        continue;

                    dev_no = sal_atoi(token);
                    kaps_sassert(dev_no < num_devices);

                    if (count == 0)
                    {
                        info->bc_dbs[dev_no] = info->db.db;
                        count++;
                        continue;
                    }

                    status = kaps_db_clone(parent->bc_dbs[dev_no], info->db.db->tid, &info->bc_dbs[dev_no]);
                    if (status != KAPS_OK)
                    {
                        print_error("Error :%d, While creating the broadcast db: %s\n", line_no,
                                    kaps_get_status_string(status));
                        return status;
                    }
                }
            }
            xmlFree(content);
        }
    }
    return KAPS_OK;
}

static kaps_status
alloc_table(
    struct xml_parse_info **list_head,
    const char *fname,
    struct kaps_device **bc_devices,
    uint32_t num_devices,
    struct kaps_device *device,
    xmlNode * node,
    int32_t dev_no,
    uint32_t id,
    uint32_t capacity,
    uint32_t width,
    enum kaps_db_type type,
    struct xml_parse_info **parse_info)
{
    uint32_t line_no = node->line;
    kaps_status status;
    struct xml_parse_info *info;

    rop_status = KAPS_OK;

    /*
     * Create the database for first specified device number 
     */
    if (dev_no != -1)
        device = bc_devices[dev_no];

    info = kaps_syscalloc(1, sizeof(*info));
    if (info == NULL)
    {
        print_error("%s:%d Parser ran out of memory\n", fname, line_no);
        *parse_info = NULL;
        return KAPS_OUT_OF_MEMORY;
    }

    info->type = type;
    info->next = NULL;
    info->line_no = line_no;
    info->id = id;
    info->algorithmic = 1;

    if (*list_head == NULL)
    {
        *list_head = info;
    }
    else
    {
        struct xml_parse_info *tmp = *list_head;

        while (tmp->next != NULL)
            tmp = tmp->next;
        tmp->next = info;
    }

    if (type == KAPS_DB_AD)
    {
        status = kaps_ad_db_init(device, id, capacity, width, &info->db.ad_db);
    }
    else if (type == KAPS_DB_DMA)
    {
        status = kaps_dma_db_init(device, id, capacity, width, &info->db.dma_db);
    }
    else
    {
        status = kaps_db_init(device, type, id, capacity, &info->db.db);
    }

    if (status != KAPS_OK)
    {
        print_error("%s:%d %s\n", fname, line_no, kaps_get_status_string(status));

        if (device && device->is_rop_mode)
            rop_status = status;

        return status;
    }

    if (type == KAPS_DB_ACL || type == KAPS_DB_LPM || type == KAPS_DB_EM)
    {
        status = alloc_bc_databases(node, info, bc_devices, num_devices);
        if (status != KAPS_OK)
        {
            print_error("%s:%d %s\n", fname, line_no, kaps_get_status_string(status));
            if (device->is_rop_mode)
                rop_status = status;
            return status;
        }
    }

    *parse_info = info;
    return KAPS_OK;
}

static struct xml_parse_info *
alloc_sub_table(
    const char *fname,
    uint32_t line_no,
    xmlNode * node,
    struct kaps_device **bc_devices,
    uint32_t num_devices,
    uint32_t id,
    struct xml_parse_info *parent,
    int32_t is_clone)
{
    struct xml_parse_info *info;
    kaps_status status;

    info = kaps_syscalloc(1, sizeof(*info));
    if (info == NULL)
    {
        print_error("%s:%d Parser ran out of memory\n", fname, line_no);
        return NULL;
    }

    info->type = parent->type;
    info->next = NULL;
    info->line_no = line_no;
    info->id = id;
    if (!is_clone)
    {
        info->next_tab = parent->next_tab;
        parent->next_tab = info;
        parent->container_db = 1;
        status = kaps_db_add_table(parent->db.db, id, &info->db.db);
    }
    else
    {
        status = kaps_db_clone(parent->db.db, id, &info->db.db);
        if (status == KAPS_OK)
        {
            info->next = parent->clone;
            parent->clone = info;

            if (parent->db.db->type == KAPS_DB_ACL
                || parent->db.db->type == KAPS_DB_LPM || parent->db.db->type == KAPS_DB_EM)
            {
                status = clone_bc_databases(node, parent, info, bc_devices, num_devices);
                if (status != KAPS_OK)
                {
                    print_error("%s:%d %s\n", fname, line_no, kaps_get_status_string(status));
                    kaps_sysfree(info);
                    return NULL;
                }
            }
        }
    }
    if (status != KAPS_OK)
    {
        print_error("%s:%d %s\n", fname, line_no, kaps_get_status_string(status));
        kaps_sysfree(info);
        return NULL;
    }

    return info;
}

static struct xml_inst_info *
alloc_instruction(
    struct xml_inst_info **list_head,
    const char *fname,
    struct kaps_device *device,
    uint32_t line_no,
    uint32_t id,
    uint32_t ltr)
{
    kaps_status status;
    struct xml_inst_info *info;

    info = kaps_syscalloc(1, sizeof(*info));
    if (info == NULL)
    {
        print_error("%s:%d Parser ran out of memory\n", fname, line_no);
        return NULL;
    }

    if (*list_head == NULL)
    {
        *list_head = info;
    }
    else
    {
        struct xml_inst_info *tmp = *list_head;

        while (tmp->next != NULL)
            tmp = tmp->next;
        tmp->next = info;
    }

    info->bc_dev_no = device->bc_id;
    status = kaps_instruction_init(device, id, ltr, &info->instruction);
    if (status != KAPS_OK)
    {
        print_error("%s:%d %s\n", fname, line_no, kaps_get_status_string(status));
        return NULL;
    }

    return info;
}

static int32_t
find_int_field(
    const char *fname,
    xmlNode * node,
    const char *field)
{
    int32_t value = -1;

    for (; node; node = node->next)
    {
        xmlChar *content;
        if (node->type != XML_ELEMENT_NODE)
            continue;
        content = xmlNodeGetContent(node);
        if (!content)
            continue;
        if (xmlStrcasecmp(node->name, BAD_CAST field) == 0)
        {
            value = parse_int(fname, node->line, content);
            xmlFree(content);
            break;
        }
        xmlFree(content);
    }
    return value;
}

static int32_t
find_smt_thread(
    const char *fname,
    xmlNode * node,
    const char *field,
    int32_t expect_smt)
{
    int32_t value = -1, found = 0;

    for (; node; node = node->next)
    {
        xmlChar *content;
        if (node->type != XML_ELEMENT_NODE)
            continue;
        content = xmlNodeGetContent(node);
        if (!content)
            continue;
        if (xmlStrcasecmp(node->name, BAD_CAST field) == 0)
        {
            if (!expect_smt)
            {
                print_error("%s:%d Thread ID specification ignored as device is not in SMT mode\n", fname, node->line);
                value = 0;
                xmlFree(content);
                break;
            }

            value = parse_int(fname, node->line, content);
            if (value < 0 || (value > 1))
            {
                print_error("%s:%d Thread ID specification is invalid\n", fname, node->line);
                value = -1;
            }
            xmlFree(content);
            found = 1;
            break;
        }
        xmlFree(content);
    }

    if (expect_smt && found == 0)
    {
        print_error("%s Expect thread identifier as SMT is enabled\n", fname);
        value = -1;
    }

    return value;
}

static kaps_status
parse_default_prefixes(
    const char *fname,
    struct kaps_device *device,
    xmlNode * node,
    struct xml_parse_info *info)
{
    xmlChar *priority;
    int32_t prio;
    xmlNode *tmp;
    struct xml_parse_default_entries *pfxs = NULL;
    uint32_t count = 0;
    uint8_t *data_mask, *ptr;
    int32_t i, step;
    kaps_status status = KAPS_OK;

    priority = xmlGetProp(node, BAD_CAST "priority");
    if (priority == NULL)
    {
        print_error("%s:%d Require priority for Default Prefixes\n", fname, node->line);
        return KAPS_PARSE_ERROR;
    }
    prio = parse_int(fname, node->line, priority);
    xmlFree(priority);

    if (prio < KAPS_ENTRY_META_PRIORITY_0 || prio > KAPS_ENTRY_META_PRIORITY_3)
    {
        print_error("%s:%d Invalid priority for Default Prefixes \n", fname, node->line);
        return KAPS_PARSE_ERROR;
    }

    for (tmp = node->children; tmp != NULL; tmp = tmp->next)
    {
        if (tmp->type == XML_ELEMENT_NODE)
        {
            xmlChar *content = xmlNodeGetContent(tmp);

            if (content && (xmlStrcasecmp(tmp->name, BAD_CAST "prefix") == 0))
            {
                count++;
            }

            if (content)
                xmlFree(content);
        }
    }

    if (count == 0)
    {
        return KAPS_OK;
    }

    pfxs = kaps_syscalloc(1, sizeof(struct xml_parse_default_entries));

    if (!pfxs)
        return KAPS_OUT_OF_MEMORY;

    pfxs->priority = prio;
    pfxs->nentries = count;
    pfxs->entries = kaps_syscalloc(count, sizeof(struct kaps_parse_record));

    data_mask = kaps_syscalloc(1, 2 * count * (info->width_1 / KAPS_BITS_IN_BYTE));
    step = info->width_1 / KAPS_BITS_IN_BYTE;
    if (data_mask == NULL)
    {
        kaps_sysfree(pfxs->entries);
        kaps_sysfree(pfxs);
        return KAPS_OUT_OF_MEMORY;
    }
    ptr = data_mask;
    for (i = 0; i < count; i++)
    {
        pfxs->entries[i].data = ptr;
        ptr += step;
        pfxs->entries[i].mask = ptr;
        kaps_memset(ptr, -1, step);
        ptr += step;
    }

    if (!pfxs->entries)
    {
        kaps_sysfree(pfxs);
        return KAPS_OUT_OF_MEMORY;
    }

    count = 0;
    for (node = node->children; node != NULL; node = node->next)
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            xmlChar *content = xmlNodeGetContent(node);

            if (content && (xmlStrcasecmp(node->name, BAD_CAST "prefix") == 0))
            {
                status = kaps_parse_entry(info->type, (const char *) (content),
                                          info->db_key, &pfxs->entries[count], info->width_1, count, 1, 0, 0, 0);

                if (status != KAPS_OK)
                {
                    kaps_parse_destroy(pfxs->entries);
                    kaps_sysfree(pfxs);
                    return status;
                }
                count++;
            }

            if (content)
                xmlFree(content);
        }
    }

    if (info->default_entries == NULL)
    {
        info->default_entries = pfxs;
    }
    else
    {
        struct xml_parse_default_entries *tmp = info->default_entries;

        while (tmp->next)
            tmp = tmp->next;
        tmp->next = pfxs;
    }
    return KAPS_OK;
}

static kaps_status
parse_construct(
    struct kaps_device *device,
    struct kaps_device **bc_devices,
    uint32_t num_devices,
    uint32 skip_clones,
    struct xml_parse_info **list,
    const char *fname,
    xmlNode * node,
    struct xml_parse_info *info)
{
    rop_status = KAPS_OK;

    for (node = node->children; node != NULL; node = node->next)
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            xmlChar *content = xmlNodeGetContent(node);
            if (content)
            {
                if (xmlStrcasecmp(node->name, BAD_CAST "device_map") == 0)
                {
                    xmlFree(content);
                    continue;
                }
                if (xmlStrcasecmp(node->name, BAD_CAST "table") == 0)
                {
                    /*
                     * Table specification inside the database, treat it as if it were another
                     * database on its own.
                     */
                    struct xml_parse_info *tab;
                    xmlChar *id;
                    int32_t tid;

                    id = xmlGetProp(node, BAD_CAST "id");
                    if (id == NULL)
                    {
                        print_error("%s:%d Require ID for table specification\n", fname, node->line);
                        return KAPS_PARSE_ERROR;
                    }
                    tid = parse_int(fname, node->line, id);
                    xmlFree(id);

                    tab = alloc_sub_table(fname, node->line, node, bc_devices, num_devices, tid, info, 0);
                    if (tab == NULL)
                        return KAPS_PARSE_ERROR;
                    tab->ad_info = info->ad_info;
                    KAPS_TRY(parse_construct(device, bc_devices, num_devices, skip_clones, list, fname, node, tab));
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "table_mix_percent") == 0)
                {
                    info->table_mix_percent = parse_int(fname, node->line, content);
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "ad_percent") == 0)
                {
                    if (info->type == KAPS_DB_AD)
                    {
                        info->table_mix_percent = parse_int(fname, node->line, content);
                    }
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "parse_only") == 0)
                {
                    info->parse_upto = parse_int(fname, node->line, content);
                    if (info->ovride && info->ovride->parse_only_valid)
                    {
                        info->parse_upto = info->ovride->parse_only;
                        info->ovride->parse_only_valid = 0;
                    }
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "est_capacity_per_mb") == 0)
                {
                    info->est_capacity_per_mb = parse_int(fname, node->line, content);
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "est_min_capacity") == 0)
                {
                    info->est_min_capacity = parse_int(fname, node->line, content);
                    if (info->ovride && info->ovride->est_min_capacity_valid)
                    {
                        info->est_min_capacity = info->ovride->est_min_capacity;
                        info->ovride->est_min_capacity_valid = 0;
                    }
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "exp_cap_range") == 0)
                {
                    xmlChar *min, *max;
                    min = xmlGetProp(node, BAD_CAST "min");
                    max = xmlGetProp(node, BAD_CAST "max");
                    if (min == NULL || max == NULL)
                    {
                        print_error("%s:%d Require expected capacity min and max specification\n", fname, node->line);
                        xmlFree(min);
                        xmlFree(max);
                        return KAPS_PARSE_ERROR;
                    }
                    info->user_cap_min = parse_int(fname, node->line, min);
                    info->user_cap_max = parse_int(fname, node->line, max);
                    xmlFree(min);
                    xmlFree(max);
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "exp_ops_range") == 0)
                {
                    xmlChar *min, *max;
                    min = xmlGetProp(node, BAD_CAST "min");
                    max = xmlGetProp(node, BAD_CAST "max");
                    if (min == NULL || max == NULL)
                    {
                        print_error("%s:%d Require expected capacity min and max specification\n", fname, node->line);
                        xmlFree(min);
                        xmlFree(max);
                        return KAPS_PARSE_ERROR;
                    }
                    info->user_ops_min = parse_int(fname, node->line, min);
                    info->user_ops_max = parse_int(fname, node->line, max);
                    xmlFree(min);
                    xmlFree(max);
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "input") == 0)
                {
                    if (info->inputs)
                    {
                        print_error("%s:%d Multiple specifications of input\n", fname, node->line);
                        return KAPS_PARSE_ERROR;
                    }
                    KAPS_TRY(expand_inputs(fname, info, (const char *) content));
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "mode") == 0)
                {
                    if (info->type != KAPS_DB_ACL)
                    {
                        print_error("%s:%d Mode specification valid for ACLs only, ignored\n", fname, node->line);
                        continue;
                    }
                    if (xmlStrcasecmp(content, BAD_CAST "batch") == 0)
                    {
                        info->incremental = 0;
                    }
                    else if (xmlStrcasecmp(content, BAD_CAST "incremental") == 0)
                    {
                        info->incremental = 1;
                    }
                    else
                    {
                        print_error("%s:%d Mode specification unrecognized\n", fname, node->line);
                        return KAPS_PARSE_ERROR;
                    }
                    if (info->ovride && info->ovride->mode_valid)
                    {
                        info->incremental = info->ovride->mode;
                        info->ovride->mode_valid = 0;
                    }
                }
                else if (xmlStrstr(node->name, (xmlChar *) "kaps_prop") != NULL)
                {
                    int32_t value = 0, i, set_flag = 0;

                    for (i = 0; i < NUM_DB_PROP; i++)
                    {
                        if (xmlStrcasecmp(node->name, BAD_CAST db_prop[i].prop_name) == 0)
                        {
                            switch (db_prop[i].property)
                            {
                                case KAPS_PROP_INDEX_CALLBACK:
                                case KAPS_PROP_SAVE_IX_SPACE:
                                case KAPS_PROP_ENTRY_META_PRIORITY:    
                                {
                                    /*
                                     * To be handled 
                                     */
                                    set_flag = 1;
                                    break;
                                }
                                case KAPS_PROP_INDEX_RANGE:
                                {
                                    xmlChar *min, *max;
                                    min = xmlGetProp(node, BAD_CAST "min");
                                    max = xmlGetProp(node, BAD_CAST "max");
                                    if (min == NULL || max == NULL)
                                    {
                                        print_error("%s:%d Require min and max specification for index range\n", fname,
                                                    node->line);
                                        xmlFree(min);
                                        xmlFree(max);
                                        return KAPS_PARSE_ERROR;
                                    }
                                    info->index_range_min = parse_int(fname, node->line, min);
                                    info->index_range_max = parse_int(fname, node->line, max);
                                    KAPS_TRY(kaps_db_set_property
                                             (info->db.db, KAPS_PROP_INDEX_RANGE, info->index_range_min,
                                              info->index_range_max));
                                    xmlFree(min);
                                    xmlFree(max);
                                    set_flag = 1;
                                    break;
                                }
                                case KAPS_PROP_DESCRIPTION:
                                {
                                    xmlChar *desc = xmlNodeGetContent(node);
                                    if (desc)
                                    {
                                        KAPS_TRY(kaps_db_set_property
                                                 (info->db.db, KAPS_PROP_DESCRIPTION, (const char *) desc));
                                        set_flag = 1;
                                        xmlFree(desc);
                                    }
                                    break;
                                }
                                case KAPS_PROP_MCOR_VALUE:
                                {
                                    int32_t r_unit, start_range, end_range;

                                    xmlChar *range_unit, *range_lo, *range_hi;
                                    range_unit = xmlGetProp(node, BAD_CAST "range_unit");
                                    range_lo = xmlGetProp(node, BAD_CAST "lo");
                                    range_hi = xmlGetProp(node, BAD_CAST "hi");
                                    if (range_unit == NULL || range_lo == NULL || range_hi == NULL)
                                    {
                                        print_error
                                            ("%s:%d Require range_unit, lo and hi specification for index range\n",
                                             fname, node->line);
                                        xmlFree(range_unit);
                                        xmlFree(range_lo);
                                        xmlFree(range_hi);
                                        return KAPS_PARSE_ERROR;
                                    }
                                    r_unit = parse_int(fname, node->line, range_unit);
                                    start_range = parse_int(fname, node->line, range_lo);
                                    end_range = parse_int(fname, node->line, range_hi);
                                    KAPS_TRY(kaps_db_set_property
                                             (info->db.db, KAPS_PROP_MCOR_VALUE, r_unit, start_range, end_range));
                                    xmlFree(range_unit);
                                    xmlFree(range_lo);
                                    xmlFree(range_hi);
                                    set_flag = 1;
                                    break;
                                }
                                case KAPS_PROP_USE_MCOR:
                                case KAPS_PROP_ALGORITHMIC:
                                case KAPS_PROP_MIN_PRIORITY:
                                case KAPS_PROP_MAX_PRIORITY:
                                case KAPS_PROP_CASCADE_DEVICE:
                                case KAPS_PROP_REDUCED_INDEX_CALLBACKS:
                                case KAPS_PROP_DMA_TAG:
                                case KAPS_PROP_MAX_CAPACITY:
                                case KAPS_PROP_ENABLE_DB_COMPACTION:
                                case KAPS_PROP_NUM_BBS:
                                case KAPS_PROP_DEFER_DELETES:
                                case KAPS_PROP_SCALE_UP_CAPACITY:
                                case KAPS_PROP_ENABLE_COUNTERS:
                                case KAPS_PROP_COUNTER_TYPE:
                                case KAPS_PROP_COUNTER_CYCLE:
                                case KAPS_PROP_XL_DB:
                                case KAPS_PROP_LOCALITY:
                                case KAPS_PROP_REPLICATE_DB:
                                case KAPS_PROP_MC_DB:
                                case KAPS_PROP_ENABLE_DYNAMIC_ALLOCATION:
                                case KAPS_PROP_UTILISE_UDA_HOLES:
                                {
                                    value = parse_int(fname, node->line, content);
                                    set_flag = 1;
                                    if (db_prop[i].property == KAPS_PROP_ALGORITHMIC)
                                    {
                                        if (info->ovride && info->ovride->algo_valid)
                                        {
                                            value = info->ovride->algo;
                                            info->ovride->algo_valid = 0;
                                        }
                                        if (value == 1 && info->type != KAPS_DB_ACL
                                            && !info->db.db->common_info->mapped_to_acl)
                                        {
                                            print_error
                                                ("%s:%d algorithmic type specification valid for ACLs only, ignored\n",
                                                 fname, node->line);
                                            continue;
                                        }
                                        if (value == 2 && info->type != KAPS_DB_ACL)
                                        {
                                            print_error
                                                ("%s:%d robust type specification valid for ACLs only, ignored\n",
                                                 fname, node->line);
                                            continue;
                                        }
                                        info->algorithmic = value;
                                    }

                                    if (db_prop[i].property == KAPS_PROP_DEFER_DELETES)
                                    {
                                        if (value == 1 && info->type != KAPS_DB_ACL
                                            && !info->db.db->common_info->mapped_to_acl)
                                        {
                                            print_error
                                                ("%s:%d defer delete type specification valid for ACLs only, ignored\n",
                                                 fname, node->line);
                                            continue;
                                        }
                                    }

                                    KAPS_TRY(kaps_db_set_property(info->db.db, db_prop[i].property, value));

                                    if (db_prop[i].property == KAPS_PROP_ENABLE_COUNTERS)
                                    {
                                        info->has_counters = 1;
                                    }

                                    break;
                                }
                                case KAPS_PROP_PAIR_WITH_DB:
                                {
                                    struct kaps_device *device = info->db.db->device;
                                    struct kaps_db *paired_db = NULL;
                                    struct kaps_c_list_iter it;
                                    struct kaps_list_node *el = NULL;

                                    value = parse_int(fname, node->line, content);
                                    set_flag = 1;

                                    /*
                                     * Go over the db list, find the db matching with the id 
                                     */
                                    kaps_c_list_iter_init(&device->db_list, &it);
                                    while ((el = kaps_c_list_iter_next(&it)) != NULL)
                                    {
                                        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

                                        if (db == info->db.db || db->type != info->db.db->type)
                                            continue;

                                        if (db->tid == value)
                                        {
                                            paired_db = db;
                                            break;
                                        }
                                    }

                                    if (paired_db == NULL)
                                        return KAPS_DB_PAIRING_NOT_POSSIBLE;

                                    KAPS_TRY(kaps_db_set_property(info->db.db, KAPS_PROP_PAIR_WITH_DB, paired_db));
                                    break;
                                }
                                case KAPS_PROP_INVALID:
                                    print_error("%s:%d Invalid HW resource Property specification\n", fname,
                                                node->line);
                                    return KAPS_PARSE_ERROR;
                                default:
                                    print_error("%s:%d Invalid HW resource Property specification\n", fname,
                                                node->line);
                                    return KAPS_PARSE_ERROR;
                            }
                        }
                    }
                    if (!set_flag)
                    {
                        print_error("%s:%d Database Property specification unrecognized\n", fname, node->line);
                        return KAPS_PARSE_ERROR;
                    }
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "type") == 0)
                {
                    if (xmlStrcasecmp(content, BAD_CAST "algorithmic") == 0)
                    {
                        if (info->type != KAPS_DB_ACL)
                        {
                            print_error("%s:%d algorithmic type specification valid for ACLs only, ignored\n",
                                        fname, node->line);
                            continue;
                        }
                        info->algorithmic = 1;
                        KAPS_TRY(kaps_db_set_property(info->db.db, KAPS_PROP_ALGORITHMIC, info->algorithmic));
                    }
                    else if (xmlStrcasecmp(content, BAD_CAST "traditional") == 0)
                    {
                        info->algorithmic = 0;
                        KAPS_TRY(kaps_db_set_property(info->db.db, KAPS_PROP_ALGORITHMIC, info->algorithmic));
                    }
                    else if (xmlStrcasecmp(content, BAD_CAST "robust") == 0)
                    {
                        if (info->type != KAPS_DB_ACL)
                        {
                            print_error("%s:%d robust type specification valid for ACLs only, ignored\n",
                                        fname, node->line);
                            continue;
                        }
                        info->algorithmic = 2;
                    }
                    else if (xmlStrcasecmp(content, BAD_CAST "index") == 0)
                    {
                        if (info->type != KAPS_DB_AD)
                        {
                            print_error("%s:%d index type specification valid for AD only, ignored\n", fname,
                                        node->line);
                            continue;
                        }
                        info->db.db->hw_res.ad_res->ad_type = KAPS_AD_TYPE_INDEX;
                        info->db.db->common_info->ad_info.db->hw_res.db_res->ad_type = KAPS_AD_TYPE_INDEX;
                    }
                    else if (xmlStrcasecmp(content, BAD_CAST "indirection") == 0)
                    {
                        if (info->type != KAPS_DB_AD)
                        {
                            print_error("%s:%d indirection type specification valid for AD only, ignored\n",
                                        fname, node->line);
                            continue;
                        }
                        info->db.db->hw_res.ad_res->ad_type = KAPS_AD_TYPE_INDIRECTION;
                        info->db.db->common_info->ad_info.db->hw_res.db_res->ad_type = KAPS_AD_TYPE_INDIRECTION;
                    }
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "capacity") == 0)
                {
                    /*
                     * Nothing already handled 
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "thread") == 0)
                {
                    /*
                     * Nothing already handled 
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "ad") == 0)
                {
                    xmlChar *id;
                    struct xml_parse_info *ad;
                    int32_t db_id, capacity, width;
                    kaps_status status;
                    struct kaps_parse_ad_info *parse_ad_info = kaps_syscalloc(1, sizeof(struct kaps_parse_ad_info));

                    if (parse_ad_info == NULL)
                        return KAPS_OUT_OF_MEMORY;

                    id = xmlGetProp(node, BAD_CAST "id");
                    if (id == NULL)
                    {
                        print_error("%s:%d Require ID for database specification\n", fname, node->line);
                        kaps_sysfree(parse_ad_info);
                        return KAPS_PARSE_ERROR;
                    }
                    db_id = parse_int(fname, node->line, id);
                    xmlFree(id);
                    capacity = find_int_field(fname, node->children, "capacity");
                    if (capacity == -1)
                        capacity = ((struct kaps_db *) info->db.db)->common_info->capacity;
                    width = find_int_field(fname, node->children, "width");
                    if (width == -1)
                    {
                        print_error("%s:%d Require width for AD specification\n", fname, node->line);
                        kaps_sysfree(parse_ad_info);
                        return KAPS_PARSE_ERROR;
                    }

                    status =
                        alloc_table(list, fname, NULL, 0, device, node, -1, db_id, capacity, width, KAPS_DB_AD, &ad);
                    if (status != KAPS_OK)
                    {
                        kaps_sysfree(parse_ad_info);

                        if (device->is_rop_mode && rop_status == KAPS_UNSUPPORTED_AD_SIZE)
                            return rop_status;

                        return status;
                    }
                    parse_ad_info->ad_db = ad->db.ad_db;
                    parse_ad_info->ad_width_8 = (width + (KAPS_BITS_IN_BYTE - 1)) / KAPS_BITS_IN_BYTE;
                    parse_ad_info->capacity = capacity;

                    info->nad_dbs++;
                    if (info->ad_info == NULL)
                    {
                        struct xml_parse_info *tmp_tab = NULL;

                        info->ad_info = parse_ad_info;

                        tmp_tab = info->clone;
                        for (; tmp_tab; tmp_tab = tmp_tab->next)
                            tmp_tab->ad_info = parse_ad_info;

                        tmp_tab = info->next_tab;
                        for (; tmp_tab; tmp_tab = tmp_tab->next_tab)
                            tmp_tab->ad_info = parse_ad_info;

                    }
                    else
                    {
                        struct kaps_parse_ad_info *tmp = info->ad_info;

                        while (tmp->next)
                            tmp = tmp->next;
                        tmp->next = parse_ad_info;
                    }

                    status = kaps_db_set_ad(info->db.db, ad->db.ad_db);
                    if (status != KAPS_OK)
                    {
                        print_error("%s:%d %s\n", fname, node->line, kaps_get_status_string(status));
                        return KAPS_PARSE_ERROR;
                    }

                    if (info->db.db->common_info->mapped_to_acl || (info->algorithmic == 2))
                    {
                        KAPS_TRY(kaps_db_set_property(info->db.db, KAPS_PROP_ALGORITHMIC, info->algorithmic));
                    }

                    ad->table_mix_percent = 0xff;
                    KAPS_TRY(parse_construct(device, bc_devices, num_devices, skip_clones, list, fname, node, ad));
                    if (ad->table_mix_percent != 0xff)
                        parse_ad_info->ad_percent = ad->table_mix_percent;
                    else
                        parse_ad_info->ad_percent = -1;
                    ad->table_mix_percent = 0;
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "width") == 0)
                {
                    if (info->type != KAPS_DB_AD && info->type != KAPS_DB_DMA)
                    {
                        print_error("%s:%d Width valid for associated and DMA data only\n", fname, node->line);
                        return KAPS_PARSE_ERROR;
                    }
                    /*
                     * already parsed 
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "key") == 0)
                {
                    struct kaps_key *key;

                    KAPS_TRY(parse_key(fname, node, device, &info->db_key, &key, &info->width_1));
                    KAPS_TRY(kaps_db_set_key(info->db.db, key));
                    if (info->bc_dbs)
                    {
                        uint32_t iter = 0;

                        for (iter = 0; iter < num_devices; iter++)
                        {
                            if (info->bc_dbs[iter] && info->bc_dbs[iter] != info->db.db)
                                KAPS_TRY(kaps_db_set_key(info->bc_dbs[iter], key));
                        }
                    }
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "index_range") == 0)
                {
                    xmlChar *min, *max;
                    min = xmlGetProp(node, BAD_CAST "min");
                    max = xmlGetProp(node, BAD_CAST "max");
                    if (min == NULL || max == NULL)
                    {
                        print_error("%s:%d Require min and max specification for index range\n", fname, node->line);
                        xmlFree(min);
                        xmlFree(max);
                        return KAPS_PARSE_ERROR;
                    }
                    info->index_range_min = parse_int(fname, node->line, min);
                    info->index_range_max = parse_int(fname, node->line, max);
                    KAPS_TRY(kaps_db_set_property(info->db.db, KAPS_PROP_INDEX_RANGE, info->index_range_min,
                                                  info->index_range_max));
                    xmlFree(min);
                    xmlFree(max);
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "prefix_disr") == 0)
                {
                    xmlChar *len, *count;
                    uint32_t length = 0, num_pfx = 0;

                    len = xmlGetProp(node, BAD_CAST "len");
                    count = xmlGetProp(node, BAD_CAST "count");
                    if (len == NULL || count == NULL)
                    {
                        print_error("%s:%d Require len and count specification for prefix_disr\n", fname, node->line);
                        xmlFree(len);
                        xmlFree(count);
                        return KAPS_PARSE_ERROR;
                    }
                    length = parse_int(fname, node->line, len);
                    num_pfx = parse_int(fname, node->line, count);

                    if (length > KAPS_LPM_KEY_MAX_WIDTH_1)
                    {
                        print_error("%s:%d Require len <= 160 for prefix_disr\n", fname, node->line);
                        return KAPS_PARSE_ERROR;
                    }
                    if (info->pfx_disr.max_pfx_len < length)
                        info->pfx_disr.max_pfx_len = length;

                    info->pfx_disr.count[length] = num_pfx;
                    /*
                     * kaps_printf("\n -> Arr_Size: %d, len :%d count: %d", (info->pfx_disr.max_pfx_len+1), length,
                     * info->pfx_disr.count[length]);
                     */
                    xmlFree(len);
                    xmlFree(count);
                }
                else if (xmlStrstr(node->name, (xmlChar *) "kaps_hw_resource") != NULL)
                {
                    int32_t i, set_flag = 0;

                    for (i = 0; i < NUM_HW_RXC_PROP; i++)
                    {
                        if (xmlStrcasecmp(node->name, BAD_CAST hw_rxc_prop[i].prop_name) == 0)
                        {
                            switch (hw_rxc_prop[i].property)
                            {
                                case KAPS_HW_RESOURCE_DBA:
                                {
                                    info->set_num_ab = parse_int(fname, node->line, content);
                                    if (info->set_num_ab < 1 && info->set_num_ab > 256)
                                    {
                                        print_error("%s:%d AB:%d, Require AB (1-256) within range\n", fname, node->line,
                                                    info->set_num_ab);
                                        return KAPS_PARSE_ERROR;
                                    }
                                    KAPS_TRY(kaps_db_set_resource(info->db.db, KAPS_HW_RESOURCE_DBA, info->set_num_ab));
                                    set_flag = 1;
                                    break;
                                }
                                case KAPS_HW_RESOURCE_UDA:
                                {
                                    info->set_uda_mb = parse_int(fname, node->line, content);
                                    set_flag = 1;
                                    {
                                        print_error("%s:%d Invalid device type for UDA property\n", fname, node->line);
                                        return KAPS_PARSE_ERROR;
                                    }
                                    break;
                                }
                                case KAPS_HW_RESOURCE_RANGE_UNITS:
                                {
                                    uint32_t range_units = parse_int(fname, node->line, content);

                                    KAPS_TRY(kaps_db_set_resource
                                             (info->db.db, KAPS_HW_RESOURCE_RANGE_UNITS, range_units));
                                    set_flag = 1;
                                    break;
                                }
                                case KAPS_HW_RESOURCE_INVALID:
                                    print_error("%s:%d Invalid HW resource Property specification\n", fname,
                                                node->line);
                                    return KAPS_PARSE_ERROR;
                            }
                        }
                    }
                    if (!set_flag)
                    {
                        print_error("%s:%d HW resource Property specification unrecognized\n", fname, node->line);
                        return KAPS_PARSE_ERROR;
                    }
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "dba") == 0)
                {
                    info->set_num_ab = find_int_field(fname, node, "dba");
                    if (info->set_num_ab < 1 && info->set_num_ab > 256)
                    {
                        print_error("%s:%d Require AB (1-256) within range\n", fname, info->set_num_ab);
                        return KAPS_PARSE_ERROR;
                    }
                    KAPS_TRY(kaps_db_set_resource(info->db.db, KAPS_HW_RESOURCE_DBA, info->set_num_ab));
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "dba_core0") == 0)
                {
                    info->set_num_ab_core0 = find_int_field(fname, node, "dba_core0");
                    KAPS_TRY(kaps_db_set_resource(info->db.db, KAPS_HW_RESOURCE_DBA_CORE, info->set_num_ab_core0, 0));
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "dba_core1") == 0)
                {
                    info->set_num_ab_core1 = find_int_field(fname, node, "dba_core1");
                    KAPS_TRY(kaps_db_set_resource(info->db.db, KAPS_HW_RESOURCE_DBA_CORE, info->set_num_ab_core1, 1));
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "uda") == 0)
                {
                    info->set_uda_mb = find_int_field(fname, node, "uda");
                    {
                        print_error("%s:%d Invalid device type for UDA property\n", fname, node->line);
                        return KAPS_PARSE_ERROR;
                    }
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "enable_dynamic_allocation") == 0)
                {
                    int32_t value;

                    value = find_int_field(fname, node, "enable_dynamic_allocation");
                    if (value < 0 || value > 1)
                    {
                        print_error("%s:%d Require value to be either 0 or 1\n", fname, value);
                        return KAPS_PARSE_ERROR;
                    }
                    KAPS_TRY(kaps_db_set_property
                             (info->db.db, (enum kaps_db_properties) KAPS_PROP_ENABLE_DYNAMIC_ALLOCATION, value));
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "uda_core0") == 0)
                {
                    info->set_uda_mb_core0 = find_int_field(fname, node, "uda_core0");
                    KAPS_TRY(kaps_db_set_resource(info->db.db, KAPS_HW_RESOURCE_UDA_CORE, info->set_uda_mb_core0, 0));
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "uda_core1") == 0)
                {
                    info->set_uda_mb_core1 = find_int_field(fname, node, "uda_core1");
                    KAPS_TRY(kaps_db_set_resource(info->db.db, KAPS_HW_RESOURCE_UDA_CORE, info->set_uda_mb_core1, 1));
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "max_uda_lsn_size") == 0)
                {
                    int32_t value;

                    value = find_int_field(fname, node, "max_uda_lsn_size");
                    if ((value < 1) || (value > (2 * KAPS_MAX_UDCS))
                        || ((value > KAPS_MAX_UDCS) && (info->db.db->key->width_1 <= 320)))
                    {
                        print_error("%s:%d Require value to be within 1 and %d\n", fname, value, KAPS_MAX_UDCS);
                        return KAPS_PARSE_ERROR;
                    }
                    KAPS_TRY(kaps_db_set_property
                             (info->db.db, (enum kaps_db_properties) KAPS_PROP_MAX_UDA_LSN_SIZE, value));
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "description") == 0)
                {
                    xmlChar *desc = xmlNodeGetContent(node);
                    if (desc)
                    {
                        KAPS_TRY(kaps_db_set_property(info->db.db, KAPS_PROP_DESCRIPTION, (const char *) desc));
                        xmlFree(desc);
                    }
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "clone") == 0)
                {
                    if (skip_clones)
                    {
                        /*
                         * Do nothing
                         */
                    }
                    else
                    {
                        xmlChar *id;
                        int32_t tid;
                        struct xml_parse_info *tab;

                        id = xmlGetProp(node, BAD_CAST "id");
                        if (id == NULL)
                        {
                            print_error("%s:%d Require ID for clone specification\n", fname, node->line);
                            return KAPS_PARSE_ERROR;
                        }
                        tid = parse_int(fname, node->line, id);
                        xmlFree(id);

                        tab = alloc_sub_table(fname, node->line, node, bc_devices, num_devices, tid, info, 1);
                        if (tab == NULL)
                            return KAPS_PARSE_ERROR;
                        tab->ad_info = info->ad_info;
                        KAPS_TRY(parse_construct(device, bc_devices, num_devices, skip_clones, list, fname, node, tab));
                    }
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "cascade_device") == 0)
                {
                    uint32_t dev_id = parse_int(fname, node->line, content);

                    KAPS_TRY(kaps_db_set_property(info->db.db, KAPS_PROP_CASCADE_DEVICE, dev_id));
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "reduced_index_callbacks") == 0)
                {
                    uint32_t dynamic_alloc = parse_int(fname, node->line, content);

                    KAPS_TRY(kaps_db_set_property(info->db.db, KAPS_PROP_REDUCED_INDEX_CALLBACKS, dynamic_alloc));
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "kaps_default_entries") == 0)
                {
                    if (info->type == KAPS_DB_LPM)
                    {
                        KAPS_STRY(parse_default_prefixes(fname, device, node, info));
                    }
                    else
                    {
                        print_error("%s:%d Default entries are for LPM/ACL %s\n", fname, node->line, node->name);
                        return KAPS_PARSE_ERROR;
                    }
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "kaps_db_priority") == 0)
                {
                    if (info->type == KAPS_DB_LPM)
                    {
                        int32_t db_priority = parse_int(fname, node->line, content);

                        if (db_priority > 3)
                        {
                            print_error("%s:%d Invalid Priority Type for LPM %s\n", fname, node->line, node->name);
                            return KAPS_PARSE_ERROR;
                        }

                        info->db_default_priority = db_priority;
                        KAPS_STRY(kaps_db_set_property(info->db.db, KAPS_PROP_ENTRY_META_PRIORITY, db_priority));
                    }
                    else
                    {
                        print_error("%s:%d Default Prefixes are for LPM %s\n", fname, node->line, node->name);
                        return KAPS_PARSE_ERROR;
                    }
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "hb") == 0)
                {
                    xmlChar *id;
                    int32_t db_id, capacity, age;
                    kaps_status status;

                    info->hb_db_info = kaps_syscalloc(1, sizeof(struct xml_parse_hb_db_info));

                    if (info->hb_db_info == NULL)
                        return KAPS_OUT_OF_MEMORY;

                    id = xmlGetProp(node, BAD_CAST "id");
                    if (id == NULL)
                    {
                        print_error("%s:%d Require ID for database specification\n", fname, node->line);
                        kaps_sysfree(info->hb_db_info);
                        return KAPS_PARSE_ERROR;
                    }

                    db_id = parse_int(fname, node->line, id);
                    xmlFree(id);
                    capacity = find_int_field(fname, node->children, "capacity");
                    if (capacity == -1)
                    {
                        print_error("%s:%d Require capacity for HB_DB specification\n", fname, node->line);
                        kaps_sysfree(info->hb_db_info);
                        return KAPS_PARSE_ERROR;
                    }

                    age = find_int_field(fname, node->children, "age_count");
                    if (age == -1)
                    {
                        print_error("%s:%d Require age for HB_DB specification\n", fname, node->line);
                        kaps_sysfree(info->hb_db_info);
                        return KAPS_PARSE_ERROR;
                    }

                    info->hb_db_info->age = age;
                    info->hb_db_info->capacity = capacity;

                    status = kaps_hb_db_init(device, db_id, capacity, &info->hb_db_info->hb_db);
                    if (status != KAPS_OK)
                    {
                        print_error("%s:%d HB DB Creation Failed \n", fname, node->line);
                        kaps_sysfree(info->hb_db_info);
                        info->hb_db_info = NULL;
                        return KAPS_PARSE_ERROR;
                    }

                    KAPS_TRY(kaps_hb_db_set_property(info->hb_db_info->hb_db, KAPS_PROP_AGE_COUNT, age));
                    KAPS_TRY(kaps_db_set_hb(info->db.db, info->hb_db_info->hb_db));
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "ptr_0") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "ptr_1") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "ptr_2") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "ptr_3") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "sia_0") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "sia_1") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "sia_2") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "sia_3") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "sia_4") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "sia_5") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "sia_6") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "sia_7") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "sia_8") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "attribute_0") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else if (xmlStrcasecmp(node->name, BAD_CAST "attribute_1") == 0)
                {
                    /*
                     * do -nothing
                     */
                }
                else
                {
                    print_error("%s:%d Invalid specification %s\n", fname, node->line, node->name);
                    return KAPS_PARSE_ERROR;
                }
                xmlFree(content);
            }
        }

        /*
         * Override the xml values with the override parameter 
         */
        if (info->ovride)
        {

            if (info->ovride->mode_valid)
                info->incremental = info->ovride->mode;

            if (info->ovride->algo_valid)
            {

                info->algorithmic = info->ovride->algo;

                if (info->algorithmic == 1 && info->type != KAPS_DB_ACL && !info->db.db->common_info->mapped_to_acl)
                {
                    kaps_printf("%Algorithmic type specification valid for ACLs only, ignored\n");
                    continue;
                }
                if (info->algorithmic == 2 && info->type != KAPS_DB_ACL)
                {
                    kaps_printf("Robust type specification valid for ACLs only, ignored\n");
                    continue;
                }

                KAPS_TRY(kaps_db_set_property(info->db.db, KAPS_PROP_ALGORITHMIC, info->algorithmic));
            }

            if (info->ovride->est_min_capacity_valid)
            {
                info->est_min_capacity = info->ovride->est_min_capacity;
            }

            if (info->ovride->parse_only_valid)
            {
                info->parse_upto = info->ovride->parse_only;
            }
        }
    }
    return KAPS_OK;
}

static struct xml_parse_info *
search_for_table(
    struct xml_parse_info *list,
    enum kaps_db_type type,
    uint32_t id)
{
    struct xml_parse_info *tmp;

    for (tmp = list; tmp; tmp = tmp->next)
    {
        if (tmp->type == type && tmp->id == id)
            return tmp;
    }

    return NULL;
}

static kaps_status
parse_search_internal(
    struct xml_parse_info *list,
    const char *fname,
    xmlNode * node,
    int32_t dev_no,
    uint32 skip_clones,
    struct xml_inst_info *inst_info)
{
    uint32_t count = 0, index = 0;
    kaps_status status = KAPS_OK;

    inst_info->context_address = 0;
    for (; node != NULL; node = node->next)
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            uint32_t id, tid;
            int32_t result_id = -1;
            struct xml_parse_info *tmp = NULL;
            xmlChar *sid;
            enum kaps_db_type type = KAPS_DB_ACL;
            char *error_field_name;
            int32_t ad_start_byte = -1, ad_num_bytes = -1;

            if (xmlStrcasecmp(node->name, BAD_CAST "ltr") == 0)
            {
                continue;
            }

            if (xmlStrcasecmp(node->name, BAD_CAST "key") == 0)
            {
                continue;
            }

            if (xmlStrcasecmp(node->name, BAD_CAST "thread") == 0)
            {
                continue;
            }

            if (xmlStrcasecmp(node->name, BAD_CAST "instruction") == 0)
            {
                continue;
            }

            if (xmlStrcasecmp(node->name, BAD_CAST "empty") == 0)
            {
                count++;
                continue;
            }

            if (xmlStrcasecmp(node->name, BAD_CAST "inst_type") == 0)
            {
                continue;
            }

            if (xmlStrcasecmp(node->name, BAD_CAST "device_map") == 0)
            {
                continue;
            }

            if (xmlStrcasecmp(node->name, BAD_CAST "context_address") == 0)
            {
                xmlChar *content = xmlNodeGetContent(node);
                inst_info->context_address = parse_int(fname, node->line, content);
                xmlFree(content);
                continue;
            }

            if (xmlStrcasecmp(node->name, BAD_CAST "acl") == 0)
            {
                type = KAPS_DB_ACL;
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST "lpm") == 0)
            {
                type = KAPS_DB_LPM;
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST "em") == 0)
            {
                type = KAPS_DB_EM;
            }
            else
            {
                print_error("%s:%d Invalid specification %s\n", fname, node->line, node->name);
                return KAPS_PARSE_ERROR;
            }

            sid = xmlGetProp(node, BAD_CAST "clone");
            if (sid && skip_clones)
            {
                continue;
            }

            sid = xmlGetProp(node, BAD_CAST "id");
            if (sid == NULL)
            {
                print_error("%s:%d Require ID for database specification\n", fname, node->line);
                return KAPS_PARSE_ERROR;
            }

            id = parse_int(fname, node->line, sid);
            xmlFree(sid);
            tmp = search_for_table(list, type, id);
            if (!tmp)
            {
                print_error("%s:%d Could not find database %d specified\n", fname, node->line, id);
                return KAPS_PARSE_ERROR;
            }

            sid = xmlGetProp(node, BAD_CAST "table");
            if (sid)
            {
                tid = parse_int(fname, node->line, sid);
                xmlFree(sid);
                for (tmp = tmp->next_tab; tmp; tmp = tmp->next_tab)
                {
                    if (tmp->id == tid)
                        break;
                }
                if (!tmp)
                {
                    print_error("%s:%d Could not find table %d for database %d specified\n", fname, node->line, tid,
                                id);
                    return KAPS_PARSE_ERROR;
                }
            }

            sid = xmlGetProp(node, BAD_CAST "clone");
            if (sid)
            {
                int32_t error = 1;
                tid = parse_int(fname, node->line, sid);
                xmlFree(sid);
                for (tmp = tmp->clone; tmp; tmp = tmp->next)
                {
                    if (tmp->id == tid)
                    {
                        error = 0;
                        break;
                    }
                }
                if (error)
                {
                    print_error("%s:%d Could not find clone %d for database %d specified\n", fname, node->line, tid,
                                id);
                    return KAPS_PARSE_ERROR;
                }
            }
            status = kaps_key_verify(inst_info->instruction->master_key, ((struct kaps_db *) tmp->db.db)->key,
                                     &error_field_name);
            if (status != KAPS_OK)
            {
                print_key_verification_error(inst_info->instruction, tmp, status, error_field_name);
                return status;
            }

            sid = xmlGetProp(node, BAD_CAST "result");
            if (sid)
            {
                result_id = parse_int(fname, node->line, sid);
                xmlFree(sid);
                if (result_id > 7)
                {
                    print_error("%s:%d Result id :%d is not valid\n", fname, node->line, result_id);
                    return KAPS_PARSE_ERROR;
                }
            }

            sid = xmlGetProp(node, BAD_CAST "ad_start_byte");
            if (sid)
            {
                ad_start_byte = parse_int(fname, node->line, sid);
                xmlFree(sid);
                if (ad_start_byte != -1)
                {

                    if (ad_start_byte > 15 && inst_info->instruction->device->is_rop_mode)
                    {
                        print_error("%s:%d ad_start_byte :%d is not valid (ROP mode: 0-15)\n", fname, node->line,
                                    ad_start_byte);
                        return KAPS_PARSE_ERROR;
                    }

                    if (ad_start_byte > 31)
                    {
                        print_error("%s:%d ad_start_byte :%d is not valid (non ROP mode: 0-31)\n", fname, node->line,
                                    ad_start_byte);
                        return KAPS_PARSE_ERROR;
                    }
                }
            }

            sid = xmlGetProp(node, BAD_CAST "ad_num_bytes");
            if (sid)
            {
                ad_num_bytes = parse_int(fname, node->line, sid);
                xmlFree(sid);
                if (ad_num_bytes != -1)
                {

                    if (ad_num_bytes > 16 && inst_info->instruction->device->is_rop_mode)
                    {
                        print_error("%s:%d ad_num_bytes :%d is not valid (ROP mode: 16B)\n", fname, node->line,
                                    ad_num_bytes);
                        return KAPS_PARSE_ERROR;
                    }

                    if (ad_num_bytes > 32)
                    {
                        print_error("%s:%d ad_num_bytes :%d is not valid (non ROP mode: 32B)\n", fname, node->line,
                                    ad_num_bytes);
                        return KAPS_PARSE_ERROR;
                    }
                }

                if ((ad_start_byte != -1) && (ad_num_bytes != -1))
                {

                    if ((ad_start_byte + ad_num_bytes) > 16 && inst_info->instruction->device->is_rop_mode)
                    {
                        print_error
                            ("%s:%d ad_start_byte :%d ad_num_bytes id :%d is not valid (ROP mode: 16B process len )\n",
                             fname, node->line, ad_start_byte, ad_num_bytes);
                        return KAPS_PARSE_ERROR;
                    }

                    if ((ad_start_byte + ad_num_bytes) > 32)
                    {
                        print_error
                            ("%s:%d ad_start_byte :%d ad_num_bytes id :%d is not valid (non ROP mode: 32B process len )\n",
                             fname, node->line, ad_start_byte, ad_num_bytes);
                        return KAPS_PARSE_ERROR;
                    }

                }
            }

            inst_info->desc[index].result_id = (result_id != -1) ? result_id : count;
            inst_info->desc[index].db_info = tmp;

            inst_info->num_searches++;
            status = kaps_instruction_add_db(inst_info->instruction,
                                             (dev_no == -1) ? tmp->db.db : tmp->bc_dbs[dev_no],
                                             (result_id != -1) ? result_id : count);
            if (status != KAPS_OK)
            {
                print_error("%s:%d %s\n", fname, node->line, kaps_get_status_string(status));
                return status;
            }
            tmp->ninstructions++;
            if (result_id == -1)
                count++;
            index++;
        }
    }
    return KAPS_OK;
}

static kaps_status
prepare_instruction(
    struct xml_parse_info *list,
    const char *fname,
    xmlNode * node,
    struct kaps_device *device,
    uint32 skip_clones,
    struct xml_inst_info *inst_info,
    uint32_t id,
    int32_t dev_no)
{
    xmlNode *tmp;
    kaps_status status;
    int32_t found_key;

    /*
     * conflict ids 
     */
    for (tmp = node->children; tmp != NULL; tmp = tmp->next)
    {
        if (tmp->type == XML_ELEMENT_NODE)
        {
            if (xmlStrcasecmp(tmp->name, BAD_CAST "conflict") == 0)
            {
                char *buf = (char *) xmlNodeGetContent(tmp);
                char *token;
                uint32_t count = 0;

                for (token = kaps_strsep(&buf, ","); token; token = kaps_strsep(&buf, ","))
                {

                    while (sal_isspace(*token))
                        token++;
                    if (*token == '\0')
                        continue;
                    count++;
                }
                xmlFree(buf);

                inst_info->conflict_ids = kaps_syscalloc(count, sizeof(uint32_t));
                if (inst_info->conflict_ids == NULL)
                    return KAPS_OUT_OF_MEMORY;

                inst_info->num_conflicts = count;
                count = 0;
                buf = (char *) xmlNodeGetContent(tmp);
                for (token = kaps_strsep(&buf, ","); token; token = kaps_strsep(&buf, ","))
                {

                    while (sal_isspace(*token))
                        token++;
                    if (*token == '\0')
                        continue;
                    inst_info->conflict_ids[count] = sal_atoi(token);
                    count++;
                }
                xmlFree(buf);
            }
        }
    }

    /*
     * Search for master key 
     */
    found_key = 0;
    for (tmp = node->children; tmp != NULL; tmp = tmp->next)
    {
        if (tmp->type == XML_ELEMENT_NODE)
        {
            if (xmlStrcasecmp(tmp->name, BAD_CAST "key") == 0)
            {
                struct kaps_key *key;
                uint16_t master_key_width_1 = 0;

                KAPS_TRY(parse_key(fname, tmp, device, &inst_info->master_key, &key, &master_key_width_1));
                KAPS_TRY(kaps_instruction_set_key(inst_info->instruction, key));
                inst_info->master_key_width_1 = master_key_width_1;
                found_key = 1;
                break;
            }
        }
    }

    if (!found_key)
    {
        print_error("%s:%d Missing master key specification\n", fname, node->line);
        return KAPS_PARSE_ERROR;
    }

    for (node = node->children; node != NULL; node = node->next)
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            if (xmlStrcasecmp(node->name, BAD_CAST "key") == 0)
            {
                continue;
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST "ltr") == 0)
            {
                continue;
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST "inst_type") == 0)
            {
                continue;
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST "conflict") == 0)
            {
                continue;
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST "device_map") == 0)
            {
                continue;
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST "instruction") == 0)
            {
                continue;
            }
            else if (xmlStrcasecmp(node->name, BAD_CAST "context_address") == 0)
            {
                continue;
            }
            else
            {
                KAPS_STRY(parse_search_internal(list, fname, node, dev_no, skip_clones, inst_info));
                break;
            }
        }
    }
    if (!node)
        return KAPS_INTERNAL_ERROR;
    status = kaps_instruction_install(inst_info->instruction);
    if (status != KAPS_OK)
    {
        int i;

        for (i = 0; i < inst_info->instruction->num_searches; i++)
        {
            inst_info->instruction->desc[i].db = NULL;
        }
        print_error("%s:%d Instruction install failed with %s\n", fname, node->line, kaps_get_status_string(status));
        return status;
    }

    return KAPS_OK;
}

static kaps_status
parse_search(
    struct xml_inst_info **list_head,
    struct xml_parse_info *list,
    const char *fname,
    struct kaps_device **bc_devices,
    uint32_t num_devices,
    uint32_t skip_clones,
    xmlNode * node,
    uint32_t thread_id,
    struct kaps_device *device,
    uint32_t * next_ltr_id)
{
    xmlChar *char_id, *device_map_tag = NULL;
    xmlNode *tmp;
    int32_t id, ltr = -1, implicit_ltr = 0;
    enum instruction_type inst_type = INSTR_UNKNOWN;
    struct xml_inst_info *inst_info = NULL;
    char *device_map = NULL;
    char *token = NULL;

    char_id = xmlGetProp(node, BAD_CAST "id");
    if (char_id == NULL)
    {
        print_error("%s:%d Require ID for search specification\n", fname, node->line);
        return KAPS_PARSE_ERROR;
    }

    id = parse_int(fname, node->line, char_id);
    xmlFree(char_id);

    /*
     * Search for LTR number if present 
     */
    for (tmp = node->children; tmp != NULL; tmp = tmp->next)
    {
        if (tmp->type == XML_ELEMENT_NODE)
        {
            if (xmlStrcasecmp(tmp->name, BAD_CAST "ltr") == 0)
            {
                xmlChar *content = xmlNodeGetContent(tmp);
                ltr = parse_int(fname, tmp->line, content);
                xmlFree(content);
            }

            if (xmlStrcasecmp(tmp->name, BAD_CAST "device_map") == 0)
            {
                device_map_tag = xmlNodeGetContent(tmp);
                device_map = (char *) device_map_tag;
            }
        }
    }

    for (tmp = node->children; tmp != NULL; tmp = tmp->next)
    {
        if (tmp->type == XML_ELEMENT_NODE)
        {
            if (xmlStrcasecmp(tmp->name, BAD_CAST "inst_type") == 0)
            {
                xmlChar *content = xmlNodeGetContent(tmp);
                if (xmlStrcasecmp(content, BAD_CAST "cbwlpm") == 0)
                    inst_type = INSTR_CBWLPM;
                else if (xmlStrcasecmp(content, BAD_CAST "compare1") == 0)
                    inst_type = INSTR_COMPARE1;
                else if (xmlStrcasecmp(content, BAD_CAST "compare2") == 0)
                    inst_type = INSTR_COMPARE2;
                else if (xmlStrcasecmp(content, BAD_CAST "compare3") == 0)
                    inst_type = INSTR_COMPARE3;
                else
                    inst_type = INSTR_UNKNOWN;
                xmlFree(content);
                break;
            }
        }
    }

    /*
     * LTR doesn't exist for KAPS. The only valid value that can
     * be passed in KAPS to LTR is 0. Initialize LTR to 0.
     * By doing this the user can skip specifying LTR number
     * for KAPS since it is irrelevant for KAPS anyway
     */

    if (device->type == KAPS_DEVICE_KAPS)
    {
        if (ltr == -1)
            ltr = 0;
        else if (ltr != 0)
        {
            print_error("%s:%d Only LTR 0 valid for KAPS\n", fname, node->line);
            return KAPS_PARSE_ERROR;
        }
    }
    else
    {
        if (ltr == -1)
        {
            ltr = *next_ltr_id;
            *next_ltr_id = ltr + 1;
            implicit_ltr = 1;
        }
    }

    if (device_map == NULL)
    {
        inst_info = alloc_instruction(list_head, fname, device, node->line, id, ltr);
        if (inst_info == NULL)
            return KAPS_PARSE_ERROR;

        inst_info->type = inst_type;

        KAPS_TRY(prepare_instruction(list, fname, node, device, skip_clones, inst_info, id, -1));

        inst_info->thread_id = thread_id;
    }

    for (token = kaps_strsep(&device_map, ","); token; token = kaps_strsep(&device_map, ","))
    {
        uint32_t dev_no = 0;

        while (sal_isspace(*token))
            token++;
        if (*token == '\0')
            continue;

        dev_no = sal_atoi(token);
        if (dev_no >= num_devices)
        {
            print_error("Error %d: Specified more than expected number of devices\n", node->line);
            return KAPS_PARSE_ERROR;
        }

        kaps_sassert(bc_devices);
        inst_info = alloc_instruction(list_head, fname, bc_devices[dev_no], node->line, id, ltr);
        if (inst_info == NULL)
            return KAPS_PARSE_ERROR;

        inst_info->type = inst_type;

        KAPS_TRY(prepare_instruction(list, fname, node, bc_devices[dev_no], skip_clones, inst_info, id, dev_no));
    }

    if (device_map_tag)
        xmlFree(device_map_tag);
    if (inst_info->instruction->type == INSTR_COMPARE3 && implicit_ltr)
    {
        *next_ltr_id = *next_ltr_id + 1;
    }

    return KAPS_OK;
}

kaps_status
xml_find_the_input_fname(
    struct xml_parse_info ** list,
    uint16_t type,
    uint16_t id,
    uint16_t tid,
    char *local_fname)
{
    struct xml_parse_info *tmp;
    tmp = *list;

    while (tmp != NULL && ((tmp->type) != type || (tmp->id) != id))
    {
        kaps_printf("searching for the input file name.\n");
        tmp = tmp->next;
    }
    if (tmp == NULL)
    {
        kaps_printf("reaching the end of the list. Failed!\n");
        return KAPS_PARSE_ERROR;
    }
    else
    {
        kaps_printf("Found it! %s\n", *(tmp->inputs));
        sal_strncpy(local_fname, *(tmp->inputs),
                    sal_strnlen(*(tmp->inputs), SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
        return KAPS_OK;
    }
}

kaps_status
xml_parse_scenarios(
    const char *fname,
    struct kaps_device * device,
    uint32 skip_clones,
    struct xml_parse_info ** list,
    struct xml_inst_info ** inst_list)
{
    xmlDoc *doc = NULL;
    xmlNode *droot = NULL;
    xmlNode *node = NULL;
    kaps_status status = KAPS_OK;
    xmlChar *desc;
    struct kaps_device **bc_devices = NULL, *tmp = device, *smt_device[4] = { NULL, NULL, NULL, NULL }, *thread = NULL;
    uint32_t num_devices = 0, i, next_ltr_id[4] = { 0, 0, 0, 0 };

    if (fname == NULL || list == NULL || inst_list == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (device == NULL)
        return KAPS_INVALID_DEVICE_PTR;

    *list = NULL;
    *inst_list = NULL;
    LIBXML_TEST_VERSION;

    doc = xmlReadFile(fname, NULL, 0);
    if (doc == NULL)
    {
        status = KAPS_INVALID_ARGUMENT;
        goto exit;
    }

    droot = xmlDocGetRootElement(doc);
    if (droot->type != XML_ELEMENT_NODE || xmlStrcasecmp(droot->name, BAD_CAST "scenario") != 0)
    {
        print_error("%s XML file must start with scenario construct\n", fname);
        status = KAPS_PARSE_ERROR;
        goto exit;
    }

    desc = xmlGetProp(droot, BAD_CAST "name");
    if (desc != NULL)
    {
        status = kaps_device_set_property(device, KAPS_DEVICE_PROP_DESCRIPTION, desc);
        if (status != KAPS_OK)
            goto exit;
        xmlFree(desc);
    }

    desc = xmlGetProp(droot, BAD_CAST "clock");
    if (desc != NULL)
    {
        if (xmlStrcasecmp(desc, BAD_CAST "half") == 0)
        {
            status = kaps_device_set_property(device, KAPS_DEVICE_PROP_CLOCK_RATE, 1);
            if (status != KAPS_OK)
                goto exit;
        }
        else
        {
            status = kaps_device_set_property(device, KAPS_DEVICE_PROP_CLOCK_RATE, 0);
            if (status != KAPS_OK)
                goto exit;
        }
        xmlFree(desc);
    }

    smt_device[0] = device;

    for (; tmp; tmp = tmp->next_bc_device)
        num_devices++;

    if (num_devices > 1)
    {
        bc_devices = kaps_syscalloc(num_devices, sizeof(struct kaps_device *));
        if (bc_devices == NULL)
        {
            status = KAPS_OUT_OF_MEMORY;
            goto exit;
        }
        tmp = device;
        i = 0;
        for (; tmp; tmp = tmp->next_bc_device)
            bc_devices[i++] = tmp;
    }

    for (node = droot->children; node != NULL; node = node->next)
    {
        enum kaps_db_type type = KAPS_DB_INVALID;
        int32_t is_search = 0;

        if (node->type != XML_ELEMENT_NODE)
            continue;

        if (xmlStrcasecmp(node->name, BAD_CAST "acl") == 0)
        {
            type = KAPS_DB_ACL;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "lpm") == 0)
        {
            type = KAPS_DB_LPM;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "em") == 0)
        {
            type = KAPS_DB_EM;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "dma") == 0)
        {
            type = KAPS_DB_DMA;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "tap") == 0)
        {
            type = KAPS_DB_TAP;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "instruction") == 0)
        {
            is_search = 1;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "instruction_stream") == 0)
        {
            /*
             * Nothing 
             */
            continue;
        }
        else if (xmlStrncasecmp(node->name, BAD_CAST "acl-entries", 11) == 0)
        {
            FILE *fp;
            char dbfilename[100];
            char *db_type = "acl";
            xmlChar *content = xmlNodeGetContent(node);
            char *id = (char *) xmlGetProp(node, BAD_CAST "id");
            char *tid = (char *) xmlGetProp(node, BAD_CAST "tid");

            if (sal_strncmp(id, tid, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0)
            {
                kaps_sprintf(dbfilename, "%s-%s-table-%s.txt", db_type, id, tid);
            }
            else
            {
                kaps_sprintf(dbfilename, "%s-%s.txt", db_type, tid);
            }
            kaps_fprintf(stdout, "\t<input>%s</input>\n", dbfilename);

            fp = kaps_fopen(dbfilename, "w");
            if (content)
            {
                kaps_fprintf(fp, "%s", content);
                xmlFree(content);
            }
            kaps_fclose(fp);
            continue;

        }
        else if (xmlStrncasecmp(node->name, BAD_CAST "lpm-entries", 11) == 0)
        {
            FILE *fp;
            char dbfilename[100];
            char *db_type = "lpm";
            xmlChar *content = xmlNodeGetContent(node);
            char *id = (char *) xmlGetProp(node, BAD_CAST "id");
            char *tid = (char *) xmlGetProp(node, BAD_CAST "tid");

            if (sal_strncmp(id, tid, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0)
            {
                kaps_sprintf(dbfilename, "%s-%s-table-%s.txt", db_type, id, tid);
            }
            else
            {
                kaps_sprintf(dbfilename, "%s-%s.txt", db_type, tid);
            }

            fp = kaps_fopen(dbfilename, "w");
            if (content)
            {
                kaps_fprintf(fp, "%s", content);
                xmlFree(content);
            }
            kaps_fclose(fp);
            continue;

        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "note") == 0)
        {
            /*
             * Nothing 
             */
            continue;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "intent") == 0)
        {
            continue;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "status") == 0)
        {
            continue;
        }
        else if (xmlStrstr(node->name, (xmlChar *) "kaps_device") != NULL)
        {
            int32_t value = 0, i, set_flag = 0;

            for (i = 0; i < NUM_DEVICE_PROP; i++)
            {
                if (xmlStrcasecmp(node->name, BAD_CAST device_prop[i].prop_name) == 0)
                {
                    switch (device_prop[i].property)
                    {
                        case KAPS_DEVICE_PROP_ADV_UDA_WRITE:
                        case KAPS_DEVICE_ADV_UDA_WRITE:
                        case KAPS_DEVICE_PRE_CLEAR_ABS:
                        case KAPS_DEVICE_PROP_INST_LATENCY:
                        case KAPS_DEVICE_PROP_LPT_MODE:
                        case KAPS_DEVICE_PROP_COUNTER_COMPRESSION:
                        case KAPS_DEVICE_PROP_TAP_NULL_REC_SIZE:
                        case KAPS_DEVICE_PROP_TAP_INGRESS_REC_SIZE:
                        case KAPS_DEVICE_PROP_TAP_EGRESS_REC_SIZE:
                        case KAPS_DEVICE_PROP_TAP_INGRESS_OPCODE_EXT_START_LOC:
                        case KAPS_DEVICE_PROP_TAP_EGRESS_OPCODE_EXT_START_LOC:
                        case KAPS_DEVICE_PROP_TAP_INGRESS_OPCODE_EXT_LEN:
                        case KAPS_DEVICE_PROP_TAP_EGRESS_OPCODE_EXT_LEN:
                        {
                            xmlChar *content = xmlNodeGetContent(node);

                            if (content)
                            {
                                value = parse_int(fname, node->line, content);
                                set_flag = 1;

                                status = (kaps_device_set_property(device, device_prop[i].property, value));
                                if (status != KAPS_OK)
                                {
                                    xmlFree(content);
                                    goto exit;
                                }
                                xmlFree(content);
                            }

                            break;
                        }
                        default:
                        {
                            print_error("%s:%d Invalid Device Property specification\n", fname, node->line);
                            status = KAPS_PARSE_ERROR;
                            goto exit;
                        }
                    }
                }
            }
            if (!set_flag)
            {
                print_error("%s:%d Device Property specification unrecognized\n", fname, node->line);
                status = KAPS_PARSE_ERROR;
                goto exit;
            }
            continue;
        }
        else
        {
            status = KAPS_PARSE_ERROR;
            print_error("%s:%d unrecognized element %s\n", fname, node->line, node->name);
            break;
        }

        if (is_search)
        {
            int32_t tid;

            tid = find_smt_thread(fname, node->children, "thread", device->flags & KAPS_DEVICE_SMT);
            if (device->flags & KAPS_DEVICE_SMT && tid == -1)
            {
                status = KAPS_PARSE_ERROR;
                goto exit;
            }
            else if (!(device->flags & KAPS_DEVICE_SMT))
                tid = 0;

            if (tid < 0 || tid > 1)
            {
                status = KAPS_PARSE_ERROR;
                goto exit;
            }

            if (!smt_device[tid])
            {
                status = kaps_device_thread_init(device, tid, &smt_device[tid]);
                if (status != KAPS_OK)
                    goto exit;
            }

            thread = smt_device[tid];
            status = parse_search(inst_list, *list, fname, bc_devices, num_devices, skip_clones,
                                  node, tid, thread, &next_ltr_id[tid]);
        }
        else
        {
            xmlChar *id;
            xmlNode *tmp_node;
            struct xml_parse_info *info;
            int32_t dev_no = -1, db_id, capacity = -1, tid;
            uint32_t width = 0;

            id = xmlGetProp(node, BAD_CAST "id");
            if (id == NULL)
            {
                print_error("%s:%d Require ID for database specification\n", fname, node->line);
                status = KAPS_PARSE_ERROR;
                goto exit;
            }
            db_id = parse_int(fname, node->line, id);
            xmlFree(id);

            capacity = find_int_field(fname, node->children, "capacity");
            if (capacity == -1)
            {
                print_error("%s:%d Require capacity in database specification\n", fname, node->line);
                status = KAPS_PARSE_ERROR;
                goto exit;
            }

            if (type == KAPS_DB_DMA)
                width = find_int_field(fname, node->children, "width");

            tid = 0;

            if (device->flags & KAPS_DEVICE_SMT && tid == -1)
            {
                status = KAPS_PARSE_ERROR;
                goto exit;
            }
            else if (!(device->flags & KAPS_DEVICE_SMT))
                tid = 0;

            if (tid < 0 || tid > 1)
            {
                status = KAPS_PARSE_ERROR;
                goto exit;
            }

            for (tmp_node = node->children; tmp_node != NULL; tmp_node = tmp_node->next)
            {
                if (tmp_node->type == XML_ELEMENT_NODE)
                {
                    xmlChar *content = xmlNodeGetContent(tmp_node);
                    if (content && xmlStrcasecmp(tmp_node->name, BAD_CAST "device_map") == 0)
                    {
                        char *buf = (char *) content;
                        char *token;

                        for (token = kaps_strsep(&buf, ","); token; token = kaps_strsep(&buf, ","))
                        {
                            while (sal_isspace(*token))
                                token++;
                            if (*token == '\0')
                                continue;

                            dev_no = sal_atoi(token);
                            break;
                        }
                    }
                    if (content)
                        xmlFree(content);
                }
            }

            if (!smt_device[tid])
            {
                status = kaps_device_thread_init(device, tid, &smt_device[tid]);
                if (status != KAPS_OK)
                {
                    kaps_printf("kaps_device_thread_init(device, tid, &smt_device[tid]) failed: %s\n",
                                kaps_get_status_string(status));
                    goto exit;
                }
            }
            thread = smt_device[tid];

            status = alloc_table(list, fname, bc_devices, num_devices, thread, node,
                                 dev_no, db_id, capacity, width, type, &info);

            if (status != KAPS_OK)
            {
                break;
            }
            info->incremental = 1;

            /*
             * For LPM databases, default is algorithmic
             */
            if (info->type == KAPS_DB_LPM)
            {
                info->algorithmic = 1;
            }

            status = parse_construct(info->db.db->device, bc_devices, num_devices, skip_clones,
                                     list, fname, node, info);
        }
        if (status != KAPS_OK)
            break;
    }

    if (status == KAPS_OK)
    {
        struct xml_parse_info *cur_db = *list;

        for (; cur_db; cur_db = cur_db->next)
        {
            int32_t prev_low = 0, total_percent = 0;
            int32_t ntables = 0, total = 0;
            struct xml_parse_info *tab = cur_db;

            if (cur_db->type != KAPS_DB_AD)
            {
                struct kaps_parse_ad_info *ad_info = cur_db->ad_info;

                for (; tab; tab = tab->next_tab)
                {
                    if (tab->table_mix_percent)
                    {
                        total_percent += tab->table_mix_percent;
                        tab->tbl_mix_percent_low = prev_low;
                        tab->tbl_mix_percent_high = tab->table_mix_percent + tab->tbl_mix_percent_low - 1;
                        prev_low = tab->tbl_mix_percent_high + 1;
                        ntables++;
                    }
                    total++;
                }

                if (ntables && (total != ntables))
                {
                    print_error("Table Mix Percentage specification is missing for a table/database\n");
                    status = KAPS_PARSE_ERROR;
                    goto exit;
                }

                if (total_percent)
                {
                    if (total_percent < 100 || total_percent > 100)
                    {
                        print_error("Total Percent: %d% => Database distribution percentage should be total of 100%\n",
                                    total_percent);
                        status = KAPS_PARSE_ERROR;
                        goto exit;
                    }
                }

                prev_low = 0;
                total_percent = 0;
                ntables = 0;
                total = 0;

                for (; ad_info; ad_info = ad_info->next)
                {
                    if (ad_info->ad_percent != -1)
                    {
                        total_percent += ad_info->ad_percent;
                        ad_info->ad_percent_index_start = prev_low;
                        ad_info->ad_percent_index_end = ad_info->ad_percent + ad_info->ad_percent_index_start - 1;
                        prev_low = ad_info->ad_percent_index_end + 1;
                        ntables++;
                    }
                    total++;
                }

                if (ntables && (total != ntables))
                {
                    print_error("AD Mix Percentage specification is missing for a ad database\n");
                    status = KAPS_PARSE_ERROR;
                    goto exit;
                }

                if (total_percent)
                {
                    if (total_percent < 100 || total_percent > 100)
                    {
                        print_error("Total Percent: %d% => Database distribution percentage should be total of 100%\n",
                                    total_percent);
                        status = KAPS_PARSE_ERROR;
                        goto exit;
                    }
                }

            }
        }

        cur_db = *list;
        for (; cur_db; cur_db = cur_db->next)
        {
            struct xml_parse_info *tab;

            if (cur_db->type == KAPS_DB_AD || cur_db->type == KAPS_DB_TAP || cur_db->type == KAPS_DB_COUNTER
                || cur_db->type == KAPS_DB_HB)
            {
                continue;
            }

            for (tab = cur_db; tab; tab = tab->next_tab)
            {
                struct xml_parse_info *ctab;

                tab->incremental = cur_db->incremental;
                tab->nad_dbs = cur_db->nad_dbs;
                tab->ad_info = cur_db->ad_info;
                tab->hb_db_info = cur_db->hb_db_info;
                for (ctab = tab->clone; ctab; ctab = ctab->next)
                {
                    ctab->incremental = cur_db->incremental;
                    ctab->nad_dbs = cur_db->nad_dbs;
                    ctab->hb_db_info = cur_db->hb_db_info;
                    ctab->ad_info = cur_db->ad_info;
                }
            }
        }
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();

    if (status != KAPS_OK)
        goto exit;

exit:
    if (bc_devices)
    {
        kaps_sysfree(bc_devices);
    }
    return status;
}

#define NEW_LINE fp == stdout ? "\n":"<br>\n"
#define WHITE_SPACE_T fp == stdout ? "  ":"&emsp; "

kaps_status
print_dynamic_core_loop_step(
    FILE * fp,
    struct test_scenario * scenario,
    int32_t step_no)
{
    int32_t target_no, operation_no;

    kaps_fprintf(fp, "step no %d%s", step_no + 1, NEW_LINE);

    if (scenario->test_steps[step_no].step_type == STEP_SPECIAL)
    {
        int32_t op_num;
        int32_t db_id, i;

        for (op_num = 0; op_num < scenario->test_steps[step_no].special_step.num_operations; op_num++)
        {
            switch (scenario->test_steps[step_no].special_step.step_operations[op_num].op_type)
            {
                case SPECIAL_STEP_OPERATION_TYPE_WARMBOOT:
                    kaps_fprintf(fp, "%s warmboot%s", WHITE_SPACE_T, NEW_LINE);
                    break;
                case SPECIAL_STEP_OPERATION_TYPE_CRASH_RECOVERY:
                    kaps_fprintf(fp, "%s crash_recovery%s", WHITE_SPACE_T, NEW_LINE);
                    break;
                case SPECIAL_STEP_OPERATION_TYPE_INSTALL:
                    kaps_fprintf(fp, "%s install: ", WHITE_SPACE_T);
                    for (i = 0; i < scenario->test_steps[step_no].special_step.step_operations[op_num].op_data.no_of_db;
                         i++)
                    {
                        db_id = scenario->test_steps[step_no].special_step.step_operations[op_num].op_data.db_ids[i];
                        if (i < scenario->test_steps[step_no].special_step.step_operations[op_num].op_data.no_of_db - 1)
                            kaps_fprintf(fp, "%d, ", db_id);
                        else
                            kaps_fprintf(fp, "%d", db_id);
                    }
                    kaps_fprintf(fp, "%s", NEW_LINE);
                    break;
                default:
                    kaps_sassert(0);
            }
        }
    }
    else
    {

        if (scenario->test_steps[step_no].target_expression.num_target_element > 1)
        {
            kaps_fprintf(fp, "%s num Targets: %d, %s%s", WHITE_SPACE_T,
                         scenario->test_steps[step_no].target_expression.num_target_element,
                         scenario->test_steps[step_no].target_expression.expression_type ==
                         LOGICAL_NONE ? "LOGICAL_NONE" : scenario->test_steps[step_no].
                         target_expression.expression_type ==
                         LOGICAL_OR ? "LOGICAL_OR" : scenario->test_steps[step_no].target_expression.expression_type ==
                         LOGICAL_AND ? "LOGICAL_AND" : "INVALID", NEW_LINE);
        }

        for (target_no = 0; target_no < scenario->test_steps[step_no].target_expression.num_target_element; target_no++)
        {
            switch (scenario->test_steps[step_no].target_expression.target_elements[target_no].target_type)
            {
                case STEP_TARGET_ITER:
                    kaps_fprintf(fp, "%s iteration, count: %d%s", WHITE_SPACE_T,
                                 scenario->test_steps[step_no].target_expression.
                                 target_elements[target_no].element_iter.iter_count, NEW_LINE);
                    break;
                case STEP_TARGET_DB_FULL:
                    kaps_fprintf(fp, "%s db full, db_id: %d%s", WHITE_SPACE_T,
                                 scenario->test_steps[step_no].target_expression.
                                 target_elements[target_no].element_db_full.db_id, NEW_LINE);
                    break;
                case STEP_TARGET_NUM_ENTRIES:
                    kaps_fprintf(fp, "%s num entries, db_id: %d, num_entries: %d%s", WHITE_SPACE_T,
                                 scenario->test_steps[step_no].target_expression.
                                 target_elements[target_no].element_num_entries.db_id,
                                 scenario->test_steps[step_no].target_expression.
                                 target_elements[target_no].element_num_entries.num_entries, NEW_LINE);
                    break;
                case STEP_TARGET_DELETE_ENTRIES:
                    kaps_fprintf(fp, "%s delete entries, db_id: %d, delete_percentage: %d%%s", WHITE_SPACE_T,
                                 scenario->test_steps[step_no].target_expression.
                                 target_elements[target_no].element_delete_entries.db_id,
                                 scenario->test_steps[step_no].target_expression.
                                 target_elements[target_no].element_delete_entries.delete_percentage, NEW_LINE);
                    break;
                case STEP_TARGET_REPEAT_STEPS:
                    kaps_fprintf(fp, "%s repeat steps: from step no: %d, num_repeat: %d%s", WHITE_SPACE_T,
                                 scenario->test_steps[step_no].target_expression.
                                 target_elements[target_no].element_repeat_steps.step_no,
                                 scenario->test_steps[step_no].target_expression.
                                 target_elements[target_no].element_repeat_steps.num_repeat, NEW_LINE);
                    break;
            }
        }

        kaps_fprintf(fp, "%s%s num Operations: %d, Total Bias: %d%s", WHITE_SPACE_T, WHITE_SPACE_T,
                     scenario->test_steps[step_no].operation_list.num_operation,
                     scenario->test_steps[step_no].operation_list.total_bias, NEW_LINE);
        for (operation_no = 0; operation_no < scenario->test_steps[step_no].operation_list.num_operation;
             operation_no++)
        {
            int i;

            switch (scenario->test_steps[step_no].operation_list.step_operation[operation_no].operation_type)
            {
                case TEST_ADD:
                    kaps_fprintf(fp, "%s%s%s add, bias: %d, db_id:", WHITE_SPACE_T, WHITE_SPACE_T, WHITE_SPACE_T,
                                 scenario->test_steps[step_no].operation_list.step_operation[operation_no].bias);
                    for (i = 0; i < scenario->test_steps[step_no].operation_list.step_operation[operation_no].num_ids;
                         i++)
                    {
                        kaps_fprintf(fp, " %d",
                                     scenario->test_steps[step_no].operation_list.
                                     step_operation[operation_no].db_instr_id[i]);
                    }
                    kaps_fprintf(fp, "%s", NEW_LINE);
                    break;
                case TEST_DELETE:
                    kaps_fprintf(fp, "%s%s%s delete, bias: %d, db_id:", WHITE_SPACE_T, WHITE_SPACE_T, WHITE_SPACE_T,
                                 scenario->test_steps[step_no].operation_list.step_operation[operation_no].bias);
                    for (i = 0; i < scenario->test_steps[step_no].operation_list.step_operation[operation_no].num_ids;
                         i++)
                    {
                        kaps_fprintf(fp, " %d",
                                     scenario->test_steps[step_no].operation_list.
                                     step_operation[operation_no].db_instr_id[i]);
                    }
                    kaps_fprintf(fp, "%s", NEW_LINE);
                    break;
                case TEST_INSTALL:
                    kaps_fprintf(fp, "%s%s%s install, bias: %d, db_id:", WHITE_SPACE_T, WHITE_SPACE_T, WHITE_SPACE_T,
                                 scenario->test_steps[step_no].operation_list.step_operation[operation_no].bias);
                    for (i = 0; i < scenario->test_steps[step_no].operation_list.step_operation[operation_no].num_ids;
                         i++)
                    {
                        kaps_fprintf(fp, " %d",
                                     scenario->test_steps[step_no].operation_list.
                                     step_operation[operation_no].db_instr_id[i]);
                    }
                    kaps_fprintf(fp, "%s", NEW_LINE);
                    break;
                case TEST_SHRINK:
                    kaps_fprintf(fp, "%s%s%s shrink, bias: %d, db_id:", WHITE_SPACE_T, WHITE_SPACE_T, WHITE_SPACE_T,
                                 scenario->test_steps[step_no].operation_list.step_operation[operation_no].bias);
                    for (i = 0; i < scenario->test_steps[step_no].operation_list.step_operation[operation_no].num_ids;
                         i++)
                    {
                        kaps_fprintf(fp, " %d",
                                     scenario->test_steps[step_no].operation_list.
                                     step_operation[operation_no].db_instr_id[i]);
                    }
                    kaps_fprintf(fp, "%s", NEW_LINE);
                    break;
                case TEST_SEARCH:
                    kaps_fprintf(fp, "%s%s%s search, bias: %d, instr_id:", WHITE_SPACE_T, WHITE_SPACE_T, WHITE_SPACE_T,
                                 scenario->test_steps[step_no].operation_list.step_operation[operation_no].bias);
                    for (i = 0; i < scenario->test_steps[step_no].operation_list.step_operation[operation_no].num_ids;
                         i++)
                    {
                        kaps_fprintf(fp, " %d",
                                     scenario->test_steps[step_no].operation_list.
                                     step_operation[operation_no].db_instr_id[i]);
                    }
                    kaps_fprintf(fp, "%s", NEW_LINE);
                    break;
                case TEST_WARMBOOT:
                    kaps_fprintf(fp, "%s%s%s warmboot, bias: %d%s", WHITE_SPACE_T, WHITE_SPACE_T, WHITE_SPACE_T,
                                 scenario->test_steps[step_no].operation_list.step_operation[operation_no].bias,
                                 NEW_LINE);
                    break;
                case TEST_CRASH_RECVRY:
                    kaps_fprintf(fp, "%s%s%s crash_recovery, bias: %d%s", WHITE_SPACE_T, WHITE_SPACE_T, WHITE_SPACE_T,
                                 scenario->test_steps[step_no].operation_list.step_operation[operation_no].bias,
                                 NEW_LINE);
                    break;

                case TEST_ADD_AND_INSTALL:
                    kaps_fprintf(fp, "%s%s%s add_and_install, bias: %d, db_id:", WHITE_SPACE_T, WHITE_SPACE_T,
                                 WHITE_SPACE_T,
                                 scenario->test_steps[step_no].operation_list.step_operation[operation_no].bias);
                    for (i = 0; i < scenario->test_steps[step_no].operation_list.step_operation[operation_no].num_ids;
                         i++)
                    {
                        kaps_fprintf(fp, " %d",
                                     scenario->test_steps[step_no].operation_list.
                                     step_operation[operation_no].db_instr_id[i]);
                    }
                    kaps_fprintf(fp, "%s", NEW_LINE);
                    break;

                default:
                    kaps_sassert(0);
                    break;
            }
        }
    }
    return KAPS_OK;
}

kaps_status
print_dynamic_core_loop_scenario(
    FILE * fp,
    struct test_scenario * scenario)
{
    int32_t step_no;

    if (fp != stdout)
        kaps_fprintf(fp, "<h4><br>");

    kaps_fprintf(fp, "%s%s****%s", NEW_LINE, NEW_LINE, NEW_LINE);
    kaps_fprintf(fp, "Dynamic Test Scenario Configuration:%s", NEW_LINE);
    kaps_fprintf(fp, "num_test_steps: %d%s", scenario->num_test_steps, NEW_LINE);

    for (step_no = 0; step_no < scenario->num_test_steps; step_no++)
    {
        kaps_fprintf(fp, "%s %s", NEW_LINE, NEW_LINE);
        KAPS_STRY(print_dynamic_core_loop_step(fp, scenario, step_no));
    }
    kaps_fprintf(fp, "****%s", NEW_LINE);

    if (fp != stdout)
        kaps_fprintf(fp, "</h4><br>");
    return KAPS_OK;
}

/**

Below is the format of a test scenario. A test scenario consists of multiple no of test_step.
Each test_step can have target and operations. A target can be of type target_iter,
target_num_entries,  and target_db_full. Operations can be of type add, delete and search.
Here is an example.

<test_scenario>
    <test_step>
        <target>
            <target_iter>
            <iter_count>100</iter_count>
            </target_iter>

            <target_num_entries>
                <db_id>0</db_id>
                <num_entries>5000000</num_entries>
            </target_num_entries>

            <target_db_full>
                <db_id>0</db_id>
            </target_db_full>

            <target_delete_entries>
                <db_id>0</db_id>
                <delete_percentage>10</delete_percentage>
            </target_delete_entries>

            <target_expression_type>logical_or</target_expression_type>
        </target>

        <operations>
            <add>
                <db_id>0,1,2</db_id>
                <bias>3</bias>
            </add>
            <delete>
                <db_id>0,1,2</db_id>
                <bias>1</bias>
            </delete>
            <search>
                <db_id>0,1,2</db_id>
                <bias>1</bias>
            </search>
            <install>
                <db_id>0,1,2</db_id>
                <bias>1</bias>
            </install>
            <shrink>
                <db_id>0,1,2</db_id>
                <bias>1</bias>
            </shrink>
            <warmboot>
                <bias>1</bias>
            </warmboot>
        </operations>
    </test_step>

    <test_step>
        <target>
            <target_repeat_steps>
                <step_no>2</step_no>
                <num_repeat>3</num_repeat>
            </target_repeat_steps>
        </target>
    </test_step>

<test_step>
    <special>
        <warmboot>1</warmboot>
        <shrink>0,1,2</shrink>
        <install>0,1,3</install>
    </special>
</test_step>

</test_scenario>

*/

kaps_status
xml_parse_dynamic_core_loop_scenario(
    const char *fname,
    struct test_scenario ** ppScenario)
{
    xmlDoc *doc = NULL;
    xmlNode *droot = NULL;
    xmlNode *step = NULL;
    struct test_scenario *pScenario;

    kaps_status status = KAPS_OK;
    int32_t test_step_no = 0;
    int32_t total_test_steps = 0;

    if (fname == NULL)
        return KAPS_INVALID_ARGUMENT;

    pScenario = kaps_syscalloc(1, sizeof(*pScenario));

    *ppScenario = pScenario;

    LIBXML_TEST_VERSION;

    doc = xmlReadFile(fname, NULL, 0);
    if (doc == NULL)
    {
        return KAPS_INVALID_ARGUMENT;
    }

    droot = xmlDocGetRootElement(doc);
    if (droot->type != XML_ELEMENT_NODE || xmlStrcasecmp(droot->name, BAD_CAST "test_scenario") != 0)
    {
        print_error("%s XML file must start with test_scenario construct\n", fname);
        return KAPS_PARSE_ERROR;
    }

    for (step = droot->children; step != NULL; step = step->next)
    {

        if (step->type != XML_ELEMENT_NODE)
            continue;

        if (step->type != XML_ELEMENT_NODE || xmlStrcasecmp(step->name, BAD_CAST "test_step") != 0)
        {
            print_error("%s test_scenario must start with test_step construct\n", fname);
            return KAPS_PARSE_ERROR;
        }
        total_test_steps++;
    }

    pScenario->test_steps = kaps_syscalloc(total_test_steps, sizeof(struct test_step));

    for (step = droot->children; step != NULL; step = step->next)
    {
        xmlNode *step_construct = NULL;

        if (step->type != XML_ELEMENT_NODE)
            continue;

        if (step->type != XML_ELEMENT_NODE || xmlStrcasecmp(step->name, BAD_CAST "test_step") != 0)
        {
            print_error("%s test_scenario must start with test_step construct\n", fname);
            return KAPS_PARSE_ERROR;
        }

        for (step_construct = step->children; step_construct != NULL; step_construct = step_construct->next)
        {
            if (step_construct->type != XML_ELEMENT_NODE)
                continue;
            if (xmlStrcasecmp(step_construct->name, BAD_CAST "target") == 0)
            {
                xmlNode *target_node = NULL;
                int8_t num_target_element = 0;

                pScenario->test_steps[test_step_no].target_expression.num_target_element = 0;
                for (target_node = step_construct->children; target_node != NULL; target_node = target_node->next)
                {
                    if (target_node->type != XML_ELEMENT_NODE)
                        continue;
                    if (xmlStrcasecmp(target_node->name, BAD_CAST "target_iter") == 0)
                    {
                        xmlNode *inner_node = NULL;
                        int32_t iter_count = -1;

                        for (inner_node = target_node->children; inner_node != NULL; inner_node = inner_node->next)
                        {
                            if (inner_node->type != XML_ELEMENT_NODE)
                                continue;
                            if (xmlStrcasecmp(inner_node->name, BAD_CAST "iter_count") == 0)
                            {
                                xmlChar *content = xmlNodeGetContent(inner_node);
                                iter_count = parse_int(fname, inner_node->line, content);
                                xmlFree(content);
                            }
                        }
                        kaps_sassert(iter_count != -1);
                        pScenario->test_steps[test_step_no].target_expression.
                            target_elements[num_target_element].target_type = STEP_TARGET_ITER;
                        pScenario->test_steps[test_step_no].target_expression.
                            target_elements[num_target_element].element_iter.iter_count = iter_count;
                        num_target_element++;
                    }
                    else if (xmlStrcasecmp(target_node->name, BAD_CAST "target_num_entries") == 0)
                    {
                        xmlNode *inner_node = NULL;
                        int8_t db_id = -1;
                        int32_t num_entries = -1;

                        for (inner_node = target_node->children; inner_node != NULL; inner_node = inner_node->next)
                        {
                            if (inner_node->type != XML_ELEMENT_NODE)
                                continue;
                            if (xmlStrcasecmp(inner_node->name, BAD_CAST "db_id") == 0)
                            {
                                xmlChar *content = xmlNodeGetContent(inner_node);
                                db_id = parse_int(fname, inner_node->line, content);
                                xmlFree(content);
                            }
                            else if (xmlStrcasecmp(inner_node->name, BAD_CAST "num_entries") == 0)
                            {
                                xmlChar *content = xmlNodeGetContent(inner_node);
                                num_entries = parse_int(fname, inner_node->line, content);
                                xmlFree(content);
                            }
                        }
                        kaps_sassert(db_id != -1);
                        kaps_sassert(num_entries != -1);
                        pScenario->test_steps[test_step_no].target_expression.
                            target_elements[num_target_element].target_type = STEP_TARGET_NUM_ENTRIES;
                        pScenario->test_steps[test_step_no].target_expression.
                            target_elements[num_target_element].element_num_entries.db_id = db_id;
                        pScenario->test_steps[test_step_no].target_expression.
                            target_elements[num_target_element].element_num_entries.num_entries = num_entries;
                        num_target_element++;
                    }
                    else if (xmlStrcasecmp(target_node->name, BAD_CAST "target_delete_entries") == 0)
                    {
                        xmlNode *inner_node = NULL;
                        int8_t db_id = -1;
                        int8_t delete_percentage = -1;

                        for (inner_node = target_node->children; inner_node != NULL; inner_node = inner_node->next)
                        {
                            if (inner_node->type != XML_ELEMENT_NODE)
                                continue;
                            if (xmlStrcasecmp(inner_node->name, BAD_CAST "db_id") == 0)
                            {
                                xmlChar *content = xmlNodeGetContent(inner_node);
                                db_id = parse_int(fname, inner_node->line, content);
                                xmlFree(content);
                            }
                            else if (xmlStrcasecmp(inner_node->name, BAD_CAST "delete_percentage") == 0)
                            {
                                xmlChar *content = xmlNodeGetContent(inner_node);
                                delete_percentage = parse_int(fname, inner_node->line, content);
                                xmlFree(content);
                            }
                        }
                        kaps_sassert(db_id != -1);
                        kaps_sassert(delete_percentage != -1);
                        pScenario->test_steps[test_step_no].target_expression.
                            target_elements[num_target_element].target_type = STEP_TARGET_DELETE_ENTRIES;
                        pScenario->test_steps[test_step_no].target_expression.
                            target_elements[num_target_element].element_delete_entries.db_id = db_id;
                        pScenario->test_steps[test_step_no].target_expression.
                            target_elements[num_target_element].element_delete_entries.delete_percentage =
                            delete_percentage;
                        num_target_element++;
                    }
                    else if (xmlStrcasecmp(target_node->name, BAD_CAST "target_db_full") == 0)
                    {
                        xmlNode *inner_node = NULL;
                        int8_t db_id = -1;

                        for (inner_node = target_node->children; inner_node != NULL; inner_node = inner_node->next)
                        {
                            if (inner_node->type != XML_ELEMENT_NODE)
                                continue;
                            if (xmlStrcasecmp(inner_node->name, BAD_CAST "db_id") == 0)
                            {
                                xmlChar *content = xmlNodeGetContent(inner_node);
                                db_id = parse_int(fname, inner_node->line, content);
                                xmlFree(content);
                            }
                        }
                        kaps_sassert(db_id != -1);
                        pScenario->test_steps[test_step_no].target_expression.
                            target_elements[num_target_element].target_type = STEP_TARGET_DB_FULL;
                        pScenario->test_steps[test_step_no].target_expression.
                            target_elements[num_target_element].element_db_full.db_id = db_id;
                        num_target_element++;
                    }
                    else if (xmlStrcasecmp(target_node->name, BAD_CAST "target_repeat_steps") == 0)
                    {
                        xmlNode *inner_node = NULL;
                        int32_t step_no = -1;
                        int32_t num_repeat = -1;

                        for (inner_node = target_node->children; inner_node != NULL; inner_node = inner_node->next)
                        {
                            if (inner_node->type != XML_ELEMENT_NODE)
                                continue;
                            if (xmlStrcasecmp(inner_node->name, BAD_CAST "step_no") == 0)
                            {
                                xmlChar *content = xmlNodeGetContent(inner_node);
                                step_no = parse_int(fname, inner_node->line, content);
                                xmlFree(content);
                            }
                            else if (xmlStrcasecmp(inner_node->name, BAD_CAST "num_repeat") == 0)
                            {
                                xmlChar *content = xmlNodeGetContent(inner_node);
                                num_repeat = parse_int(fname, inner_node->line, content);
                                xmlFree(content);
                            }
                        }
                        kaps_sassert(step_no != -1);
                        kaps_sassert(num_repeat != -1);
                        pScenario->test_steps[test_step_no].target_expression.
                            target_elements[num_target_element].target_type = STEP_TARGET_REPEAT_STEPS;
                        pScenario->test_steps[test_step_no].target_expression.
                            target_elements[num_target_element].element_repeat_steps.step_no = step_no;
                        pScenario->test_steps[test_step_no].target_expression.
                            target_elements[num_target_element].element_repeat_steps.num_repeat = num_repeat;
                        num_target_element++;
                    }
                    else if (xmlStrcasecmp(target_node->name, BAD_CAST "target_expression_type") == 0)
                    {
                        enum test_step_target_element_expression_type expression_type = LOGICAL_NONE;

                        xmlChar *content = xmlNodeGetContent(target_node);
                        if (xmlStrcasestr(content, BAD_CAST "logical_or"))
                        {
                            expression_type = LOGICAL_OR;
                        }
                        else if (xmlStrcasestr(content, BAD_CAST "logical_and"))
                        {
                            expression_type = LOGICAL_AND;
                        }
                        xmlFree(content);
                        kaps_sassert(expression_type != LOGICAL_NONE);
                        pScenario->test_steps[test_step_no].target_expression.expression_type = expression_type;
                    }
                }
                pScenario->test_steps[test_step_no].target_expression.num_target_element = num_target_element;
            }
            else if (xmlStrcasecmp(step_construct->name, BAD_CAST "operations") == 0)
            {
                xmlNode *operations_node = NULL;
                int8_t num_operations = 0;
                int32_t total_bias = 0;

                for (operations_node = step_construct->children; operations_node != NULL;
                     operations_node = operations_node->next)
                {
                    int op_flag = 0;

                    if (operations_node->type != XML_ELEMENT_NODE)
                        continue;

                    if (xmlStrcasecmp(operations_node->name, BAD_CAST "add") == 0)
                    {
                        pScenario->test_steps[test_step_no].operation_list.
                            step_operation[num_operations].operation_type = TEST_ADD;
                        op_flag = 1;
                    }
                    else if (xmlStrcasecmp(operations_node->name, BAD_CAST "delete") == 0)
                    {
                        pScenario->test_steps[test_step_no].operation_list.
                            step_operation[num_operations].operation_type = TEST_DELETE;
                        op_flag = 1;
                    }
                    else if (xmlStrcasecmp(operations_node->name, BAD_CAST "install") == 0)
                    {
                        pScenario->test_steps[test_step_no].operation_list.
                            step_operation[num_operations].operation_type = TEST_INSTALL;
                        op_flag = 1;
                    }
                    else if (xmlStrcasecmp(operations_node->name, BAD_CAST "search") == 0)
                    {
                        pScenario->test_steps[test_step_no].operation_list.
                            step_operation[num_operations].operation_type = TEST_SEARCH;
                        op_flag = 1;
                    }
                    else if (xmlStrcasecmp(operations_node->name, BAD_CAST "shrink") == 0)
                    {
                        pScenario->test_steps[test_step_no].operation_list.
                            step_operation[num_operations].operation_type = TEST_SHRINK;
                        op_flag = 1;
                    }
                    else if (xmlStrcasecmp(operations_node->name, BAD_CAST "add_and_install") == 0)
                    {
                        pScenario->test_steps[test_step_no].operation_list.
                            step_operation[num_operations].operation_type = TEST_ADD_AND_INSTALL;
                        op_flag = 1;
                    }

                    if (op_flag)
                    {
                        xmlNode *inner_node = NULL;
                        int32_t bias = -1;

                        for (inner_node = operations_node->children; inner_node != NULL; inner_node = inner_node->next)
                        {
                            if (inner_node->type != XML_ELEMENT_NODE)
                                continue;
                            if ((xmlStrcasecmp(inner_node->name, BAD_CAST "db_id") == 0)
                                || (xmlStrcasecmp(inner_node->name, BAD_CAST "instr_id") == 0))
                            {
                                char *content = (char *) xmlNodeGetContent(inner_node);
                                char *token;
                                uint32_t count = 0;

                                for (token = kaps_strsep(&content, ","); token; token = kaps_strsep(&content, ","))
                                {

                                    while (sal_isspace(*token))
                                        token++;
                                    if (*token == '\0')
                                        continue;
                                    pScenario->test_steps[test_step_no].operation_list.
                                        step_operation[num_operations].db_instr_id[count] = sal_atoi(token);
                                    count++;
                                }
                                pScenario->test_steps[test_step_no].operation_list.
                                    step_operation[num_operations].num_ids = count;
                                xmlFree(content);
                            }
                            else if (xmlStrcasecmp(inner_node->name, BAD_CAST "bias") == 0)
                            {
                                xmlChar *content = xmlNodeGetContent(inner_node);
                                bias = parse_int(fname, inner_node->line, content);
                                xmlFree(content);
                            }
                        }
                        kaps_sassert(pScenario->test_steps[test_step_no].operation_list.
                                     step_operation[num_operations].num_ids);
                        kaps_sassert(bias != -1);
                        pScenario->test_steps[test_step_no].operation_list.step_operation[num_operations].bias = bias;
                        total_bias += bias;
                        num_operations++;
                    }
                    else if (xmlStrcasecmp(operations_node->name, BAD_CAST "warmboot") == 0)
                    {
                        xmlNode *inner_node = NULL;
                        int32_t bias = -1;

                        for (inner_node = operations_node->children; inner_node != NULL; inner_node = inner_node->next)
                        {
                            if (inner_node->type != XML_ELEMENT_NODE)
                                continue;
                            if (xmlStrcasecmp(inner_node->name, BAD_CAST "bias") == 0)
                            {
                                xmlChar *content = xmlNodeGetContent(inner_node);
                                bias = parse_int(fname, inner_node->line, content);
                                xmlFree(content);
                            }
                        }
                        kaps_sassert(bias != -1);
                        pScenario->test_steps[test_step_no].operation_list.
                            step_operation[num_operations].operation_type = TEST_WARMBOOT;
                        pScenario->test_steps[test_step_no].operation_list.step_operation[num_operations].bias = bias;
                        total_bias += bias;
                        num_operations++;
                    }
                }
                pScenario->test_steps[test_step_no].operation_list.num_operation = num_operations;
                pScenario->test_steps[test_step_no].operation_list.total_bias = total_bias;
            }
            else if (xmlStrcasecmp(step_construct->name, BAD_CAST "special") == 0)
            {
                xmlNode *special_node = NULL;
                int8_t num_operations = 0;

                pScenario->test_steps[test_step_no].step_type = STEP_SPECIAL;
                pScenario->test_steps[test_step_no].special_step.num_operations = 0;

                for (special_node = step_construct->children; special_node != NULL; special_node = special_node->next)
                {
                    if (special_node->type != XML_ELEMENT_NODE)
                        continue;
                    if (xmlStrcasecmp(special_node->name, BAD_CAST "warmboot") == 0)
                    {
                        int32_t value = -1;

                        xmlChar *content = xmlNodeGetContent(special_node);
                        value = parse_int(fname, special_node->line, content);

                        if (value == 1)
                        {
                            pScenario->test_steps[test_step_no].special_step.step_operations[num_operations].op_type =
                                SPECIAL_STEP_OPERATION_TYPE_WARMBOOT;
                            num_operations++;
                        }
                        xmlFree(content);
                    }
                    else if (xmlStrcasecmp(special_node->name, BAD_CAST "crash_recovery") == 0)
                    {
                        int32_t value = -1;

                        xmlChar *content = xmlNodeGetContent(special_node);
                        value = parse_int(fname, special_node->line, content);

                        if (value == 1)
                        {
                            pScenario->test_steps[test_step_no].special_step.step_operations[num_operations].op_type =
                                SPECIAL_STEP_OPERATION_TYPE_CRASH_RECOVERY;
                            num_operations++;
                        }
                        xmlFree(content);
                    }
                    else if ((xmlStrcasecmp(special_node->name, BAD_CAST "install") == 0))
                    {
                        char *content = (char *) xmlNodeGetContent(special_node);
                        char *token;
                        uint32_t count = 0;

                        if (xmlStrcasecmp(special_node->name, BAD_CAST "install") == 0)
                            pScenario->test_steps[test_step_no].special_step.step_operations[num_operations].op_type =
                                SPECIAL_STEP_OPERATION_TYPE_INSTALL;

                        count = 0;
                        for (token = kaps_strsep(&content, ","); token; token = kaps_strsep(&content, ","))
                        {

                            while (sal_isspace(*token))
                                token++;
                            if (*token == '\0')
                                continue;
                            pScenario->test_steps[test_step_no].special_step.step_operations[num_operations].
                                op_data.db_ids[count] = sal_atoi(token);
                            count++;
                        }
                        pScenario->test_steps[test_step_no].special_step.step_operations[num_operations].
                            op_data.no_of_db = count;
                        kaps_sassert(pScenario->test_steps[test_step_no].special_step.
                                     step_operations[num_operations].op_data.no_of_db);
                        num_operations++;
                        xmlFree(content);
                    }
                }
                pScenario->test_steps[test_step_no].special_step.num_operations = num_operations;
            }
        }
        test_step_no++;
    }

    pScenario->num_test_steps = test_step_no;
    return status;
}

kaps_status
xml_parse_intent_and_status(
    const char *fname,
    char **intent,
    uint32_t * t_status)
{
    xmlDoc *doc = NULL;
    xmlNode *droot = NULL;
    xmlNode *node = NULL;
    kaps_status status = KAPS_OK;
    xmlChar *desc;

    if (fname == NULL)
        return KAPS_INVALID_ARGUMENT;

    *intent = NULL;
    *t_status = 0;

    LIBXML_TEST_VERSION;

    doc = xmlReadFile(fname, NULL, 0);
    if (doc == NULL)
    {
        return KAPS_INVALID_ARGUMENT;
    }

    droot = xmlDocGetRootElement(doc);
    if (droot->type != XML_ELEMENT_NODE || xmlStrcasecmp(droot->name, BAD_CAST "scenario") != 0)
    {
        print_error("%s XML file must start with scenario construct\n", fname);
        return KAPS_PARSE_ERROR;
    }

    desc = xmlGetProp(droot, BAD_CAST "name");
    if (desc != NULL)
    {
        /*
         * DO NOTHING 
         */
        xmlFree(desc);
    }

    for (node = droot->children; node != NULL; node = node->next)
    {
        if (node->type != XML_ELEMENT_NODE)
            continue;
        if (xmlStrcasecmp(node->name, BAD_CAST "acl") == 0)
        {
            break;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "lpm") == 0)
        {
            break;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "em") == 0)
        {
            break;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "instruction") == 0)
        {
            break;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "instruction_stream") == 0)
        {
            break;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "note") == 0)
        {
            break;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "interface") == 0)
        {
            break;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "intent") == 0)
        {
            xmlChar *desc = xmlNodeGetContent(node);
            if (desc)
            {
                *intent =
                    (char *)
                    kaps_syscalloc((sal_strnlen((char *) desc, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1),
                                   sizeof(char));
                if (*intent == NULL)
                {
                    status = KAPS_OUT_OF_MEMORY;
                    break;
                }
                sal_strncpy(*intent, (char *) desc,
                            sal_strnlen((char *) desc, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) + 1);
                xmlFree(desc);
            }
            continue;
        }
        else if (xmlStrcasecmp(node->name, BAD_CAST "status") == 0)
        {
            uint32_t status = find_int_field(fname, node, "status");
            *t_status = status;
            continue;
        }
        else
        {
            break;
        }
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return status;
}

kaps_status
xml_parse_destroy(
    struct xml_parse_info * head,
    struct xml_inst_info * inst_info)
{
    struct xml_parse_info *tmp, *next;
    struct xml_inst_info *cur_inst, *del_inst;

    if (head == NULL)
        return KAPS_OK;

    tmp = head;
    while (tmp)
    {
        int32_t i;
        struct xml_parse_info *tab, *ctab;
        struct kaps_parse_ad_info *ad_info = tmp->ad_info;

        next = tmp->next;
        for (i = 0; i < tmp->num_inputs; i++)
            kaps_sysfree(tmp->inputs[i]);
        if (tmp->inputs)
            kaps_sysfree(tmp->inputs);
        while (ad_info)
        {
            struct kaps_parse_ad_info *next = ad_info->next;

            kaps_sysfree(ad_info);
            ad_info = next;
        }
        if (tmp->hb_db_info)
            kaps_sysfree(tmp->hb_db_info);
        if (tmp->bc_dbs)
            kaps_sysfree(tmp->bc_dbs);
        tab = tmp->next_tab;
        while (tab)
        {
            struct xml_parse_info *tab_next, *ctab;

            tab_next = tab->next_tab;
            for (i = 0; i < tab->num_inputs; i++)
                kaps_sysfree(tab->inputs[i]);
            if (tab->inputs)
                kaps_sysfree(tab->inputs);
            free_xml_key(tab->db_key);
            ctab = tab->clone;
            while (ctab)
            {
                struct xml_parse_info *ctab_next;

                ctab_next = ctab->next;
                free_xml_key(ctab->db_key);
                kaps_sysfree(ctab);
                ctab = ctab_next;
            }
            kaps_sysfree(tab);
            tab = tab_next;
        }
        ctab = tmp->clone;
        while (ctab)
        {
            struct xml_parse_info *ctab_next;

            ctab_next = ctab->next;
            free_xml_key(ctab->db_key);
            kaps_sysfree(ctab);
            ctab = ctab_next;
        }
        free_xml_key(tmp->db_key);

        kaps_sysfree(tmp);
        tmp = next;
    }

    cur_inst = inst_info;
    while (cur_inst)
    {
        del_inst = cur_inst;
        cur_inst = cur_inst->next;
        free_xml_key(del_inst->master_key);
        if (del_inst->conflict_ids)
            kaps_sysfree(del_inst->conflict_ids);
        kaps_sysfree(del_inst);
    }

    return KAPS_OK;
}

static kaps_status
parse_device_config_file(
    const char *fname,
    xmlNode * node,
    enum kaps_device_type dev_type,
    struct kaps_sw_model_kaps_config *m_config)
{
    uint32_t bblocks = 0, rpbs = 0, small_bbs = 0;
    uint32_t id = 0, sub_type;
    uint32_t num_rows_in_rpb = 0, num_rows_in_bb = 0, num_rows_in_small_bb = 0;
    uint32_t profile = 0;

    kaps_printf("\n Device Configuration :-");

    for (node = node->children; node != NULL; node = node->next)
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            xmlChar *content = xmlNodeGetContent(node);
            if (content)
            {
                if (dev_type == KAPS_DEVICE_KAPS)
                {
                    if (xmlStrcasecmp(node->name, BAD_CAST "id") == 0)
                    {
                        id = find_int_field(fname, node, "id");
                        m_config->id = id;
                        if (id > KAPS_JERICHO_2_DEVICE_ID)
                        {
                            kaps_printf("\n    Invalid id                    : %d ", id);
                            return KAPS_PARSE_ERROR;
                        }
                        kaps_printf("\n    id                                : %d ", id);
                    }
                    else if (xmlStrcasecmp(node->name, BAD_CAST "sub_type") == 0)
                    {
                        sub_type = find_int_field(fname, node, "sub_type");
                        m_config->sub_type = sub_type;
                        if (sub_type > KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4)
                        {
                            kaps_printf("\n    Invalid sub_type                    : %d ", sub_type);
                            return KAPS_PARSE_ERROR;
                        }
                        kaps_printf("\n    sub_type                                : %d ", sub_type);
                    }
                    else if (xmlStrcasecmp(node->name, BAD_CAST "blocks") == 0)
                    {
                        bblocks = find_int_field(fname, node, "blocks");

                        kaps_printf("\n    Bucket blocks count                : %d ", bblocks);
                    }
                    else if (xmlStrcasecmp(node->name, BAD_CAST "rpb") == 0)
                    {
                        rpbs = find_int_field(fname, node, "rpb");
                        m_config->total_num_rpb = rpbs;

                        if (rpbs > KAPS_HW_MAX_NUM_RPB_BLOCKS)
                        {
                            m_config->total_num_rpb = KAPS_HW_MAX_NUM_RPB_BLOCKS;
                        }

                        if (rpbs % 2)
                        {
                            kaps_printf("\n    Invalid Root Pivot Blocks count    : %d (non multiple of 2)", rpbs);
                            return KAPS_PARSE_ERROR;
                        }
                        kaps_printf("\n    Root Pivot Blocks count            : %d", rpbs);
                    }
                    else if (xmlStrcasecmp(node->name, BAD_CAST "small_bb") == 0)
                    {
                        small_bbs = find_int_field(fname, node, "small_bb");
                        kaps_printf("\n    Small BB count                     : %d", small_bbs);
                    }
                    else if (xmlStrcasecmp(node->name, BAD_CAST "num_rpb_rows") == 0)
                    {
                        num_rows_in_rpb = find_int_field(fname, node, "num_rpb_rows");
                        kaps_printf("\n    Number of Rows in RPB              : %d", num_rows_in_rpb);
                    }
                    else if (xmlStrcasecmp(node->name, BAD_CAST "num_bb_rows") == 0)
                    {
                        num_rows_in_bb = find_int_field(fname, node, "num_bb_rows");
                        kaps_printf("\n    Number of Rows in BB               : %d", num_rows_in_bb);
                    }
                    else if (xmlStrcasecmp(node->name, BAD_CAST "num_small_bb_rows") == 0)
                    {
                        num_rows_in_small_bb = find_int_field(fname, node, "num_small_bb_rows");
                        kaps_printf("\n    Number of Rows in Small BB         : %d", num_rows_in_small_bb);
                    }
                    else if (xmlStrcasecmp(node->name, BAD_CAST "profile") == 0)
                    {
                        profile = find_int_field(fname, node, "profile");
                        m_config->profile = profile;
                        kaps_printf("\n    Profile : %d", profile);
                    }
                    else
                    {
                        kaps_printf("\n    Invalid token @ line no            : %d ", node->line);
                        return KAPS_PARSE_ERROR;
                    }
                }
                else
                {
                    ;   /* something to be added for the OP */
                }
                xmlFree(content);
            }   /* if content */
        }       /* node type */
    }   /* for */
    kaps_printf("\n---------------------------------------------------------\n");

    return KAPS_OK;
}

kaps_status
xml_parse_device_config_file(
    const char *fname,
    enum kaps_device_type dev_type,
    struct kaps_sw_model_kaps_config * m_config)
{
    xmlDoc *doc = NULL;
    xmlNode *droot = NULL;
    xmlNode *node = NULL;
    kaps_status status = KAPS_OK;
    xmlChar *desc;

    if (fname == NULL || m_config == NULL)
        return KAPS_INVALID_ARGUMENT;

    if (dev_type < KAPS_DEVICE_KAPS || dev_type >= KAPS_DEVICE_INVALID)
        return KAPS_UNSUPPORTED_DEVICE;

    LIBXML_TEST_VERSION;

    doc = xmlReadFile(fname, NULL, 0);
    if (doc == NULL)
    {
        return KAPS_INVALID_ARGUMENT;
    }

    droot = xmlDocGetRootElement(doc);
    if (droot->type != XML_ELEMENT_NODE || xmlStrcasecmp(droot->name, BAD_CAST "scenario") != 0)
    {
        print_error("%s XML file must start with scenario construct\n", fname);
        return KAPS_PARSE_ERROR;
    }

    desc = xmlGetProp(droot, BAD_CAST "name");
    if (desc != NULL)
    {
        kaps_printf("\n Device Desc: %s ", desc);
        xmlFree(desc);
    }

    for (node = droot->children; node != NULL; node = node->next)
    {
        uint32_t is_dev_config = 0;

        if (node->type != XML_ELEMENT_NODE)
            continue;
        if (xmlStrcasecmp(node->name, BAD_CAST "device") == 0)
        {
            is_dev_config = 1;
        }
        else
        {
            status = KAPS_PARSE_ERROR;
            print_error("%s:%d unrecognized element %s\n", fname, node->line, node->name);
            break;
        }
        if (is_dev_config)
        {
            status = parse_device_config_file(fname, node, dev_type, m_config);
            if (status != KAPS_OK)
                return status;
        }
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return status;
}
