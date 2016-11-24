/*
 * $Id: robo_diag.c,v 1.1 2011/04/18 17:10:59 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * ROBO socdiag command list
 */
#include "appl/diag/diag.h"
       
char age_robo_usage[] =
    "Parameters:  [<seconds>]\n\t"
    "   Set the hardware age timer to the indicated number of seconds.\n\t"
    "   With no parameter, displays current value.\n\t"
    "   Setting to 0 disables hardware aging\n";


extern cmd_result_t cmd_robo_age(int unit, args_t *a);

char cmd_robo_auth_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "auth <option> [args...]\n"
#else
    "auth init\n\t"
    "       - Init auth function\n\t"
    "auth detach\n\t"
    "       - Detach auth function\n\t"
    "auth mac init\n\t"
    "       - Add switch mac address from config to all ports\n\t"
    "auth mac add  [PortBitMap = <pbmp>] [MACaddress=<address>]\n\t"
    "       - Add switch mac address\n\t"
    "auth mac del  [PortBitMap = <pbmp>] [MACaddress=<address>]\n\t"
    "       - Delete switch mac address\n\t"
    "auth mac clear  [PortBitMap = <pbmp>]\n\t"
    "       - Clear all switch mac addresses\n\t"
    "auth block [PortBitMap = <pbmp>] [IngressOnly=true|false]\n\t"
    "       - Block traffic for all directions or ingress direction only\n\t"
    "auth unblock [PortBitMap = <pbmp>]\n\t"
    "       - All traffic allowed, ports in uncontrolled state\n\t"
    "auth enable [PortBitMap = <pbmp>] [LearnEnable=true|false] [AuthNum=#] \n\t"
    "                  [SaNum=<value>]\n\t"
    "          # = 0: None\n\t"
    "            = 1: Static accept\n\t"
    "            = 2: Static reject\n\t"
    "            = 3: SA num (SaNum should be set)\n\t"
    "            = 4: SA match\n\t"
    "            = 5: Extend Mode(drop if SA is unknown or violated)\n\t"
    "            = 6: Simplfy Mode(trap to management port if SA is unknown or violated)\n\t"
    "       - Authorized ports to allow traffic\n\t"
    "auth disable [PortBitMap = <pbmp>]\n\t"
    "       - Put ports back in block state\n\t"
    "auth show\n\t"
    "       - Show ports access state\n"
#endif
    ;

extern cmd_result_t cmd_robo_auth(int unit, args_t *a);

char if_robo_bpdu_usage[] =
    "Usages:\n\t"
    "  bpdu add [Index=<n>] [MACaddress=<mac>] \n\t"
    "        - Add BPDU addresses.\n\t"
    "        - only Bcm5348 provide 3 BPDUs.\n\t"
    "  bpdu del [Index=<n>]\n\t"
    "        - Delete BPDU address.\n\t"
    "  bpdu show \n\t"
    "        - Show BPDU addresses.\n";

extern cmd_result_t if_robo_bpdu(int unit, args_t *a);

char cmd_robo_cablediag_usage[] =
    "Run Cable Diagnostics on a set of ports.\n"
    "Parameter: <portbitmap>\n";

extern cmd_result_t cmd_robo_cablediag(int unit, args_t *a);

char mem_robo_cache_usage[] =
    "Parameters [+<TABLE> | -<TABLE>]\n\t"
    "If no parameters are given, displays caching status.  Otherwise,\n\t"
    "turns software caching on (+) or off (-) for specified TABLE(s).\n\t"
    "Only VLAN and SEC_MAC available. SEC_MAC0, SEC_MAC1 and SEC_MAC2\n\t"
    "are all set to SEC_MAC\n";
extern cmd_result_t mem_robo_cache(int unit, args_t *a);


char cmd_robo_clear_usage[] =
    "clear counters [PBMP] | <TABLE> ... | DEV | stats [PBMP]\n\t"
    "counters - zeroes all or some internal packet counters\n\t"
    "<TABLE> - clears specified memory table\n\t"
    "        - set any of SEC_MAC0, SEC_MAC1 and SEC_MAC2 will\n\t"
    "          clear all of the three table.\n\t"
    "DEV - Call bcm_clear on the unit to reset to known state\n\t"
    "stats - Clear the port based statistics.\n";

extern cmd_result_t cmd_robo_clear(int unit, args_t *a);


char if_robo_combo_usage[] =
    "Parameters: <ports> [copper|fiber [<option>=<value>]]\n"
    "            <ports> watch [on|off]\n\t"
    "Display or update operating parameters of copper/fiber combo PHY.\n\t"
    "Note: the 'port' command operates on the currently active medium.\n\t"
    "Example: combo ge1 fiber autoneg_enable=1 autoneg_advert=1000,pause\n\t"
    "Watch subcommand enables/disables media change notifications\n";

extern cmd_result_t if_robo_combo(int unit, args_t *a);


char cmd_robo_cos_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "cos <option> [args...]\n"
#else
    "cos clear                  - Reset COS configuration to default\n\t"
    "cos config [Numcos=#]      - Set number of active COS queues\n\t"
    "cos map [Pri=#] [Queue=#]  - Map a priority 0-7 to a COS queue\n\t"   
    "cos show                   - Show current COS config\n\t"
    "cos strict                 - Set strict queue scheduling mode\n\t"
    "cos weight [W0=#] [W1=#] ... - Set weighted round-robin queue\n\t"
    "                               scheduling mode with the specified\n\t"
    "                               weights per queue\n\t"
    "cos bandwidth [PortBitMap=<pbmp>] [Queue=#] [KbpsMIn=#]\n\t"
    "              [KbitsMaxBurst=#] [Flags=#]\n\t"
    "                           - Set cos bandwidth\n\t"
    "cos bandwidth_show         - Show current COS bandwidth config\n"
    "\n\t"
    "cos port map [PortBitMap=<pbmp>] [Pri=#] [Queue=#]\n\t"
    "                           - Map a port's priority 0-7 to a COS queue\n\t"
    "cos port show [PortBitMap=<pbmp>]\n\t"
    "                           - Show current port COS config\n"
#endif
    ;
extern cmd_result_t cmd_robo_cos(int unit, args_t *a);


char cmd_robo_counter_usage[] =
     "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "counter [options]\n"
#else
    "\nParameters: [off] [sync] [Interval=<usec>]\n\t"
    "\t[PortBitMap=<pbm>] [MIBGroup=<MibGroup>]\n\t"
    "Starts the counter collection task running every <usec>\n\t"
    "microseconds.  The task tallies software counters based on hardware\n\t"
    "values and must run often enough to avoid counter overflow.\n\t"
    "If <interval> is 0, stops the task.  If <interval> is omitted, prints\n\t"
    "the current INTERVAL.  sync reads in all the counters to synchronize\n\t"
    "'show counters' for the first time, and must be used alone.\n\t"
    "for bcm5396 select MIB Group(0-2) <MibGroup> for different counters\n"
#endif
    ;

extern cmd_result_t cmd_robo_counter(int unit, args_t *a);

char if_robo_dscp_usage[] =
    "Usages:\n"
    "  dscp pbmp [source [mapped]]\n"
    "        - map source dscp to mapped\n";

extern cmd_result_t if_robo_dscp(int unit, args_t *a);

char if_robo_dtag_usage[] =
    "Usage:\n"
    "\tdtag show <pbmp>\n"
    "\tdtag mode <pbmp> none|internal|external\n"
    "\tdtag tpid <pbmp> hex-value\n";

extern cmd_result_t if_robo_dtag(int unit, args_t *a);


char cmd_robo_dump_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "DUMP [options]\n"
#else
    "DUMP [File=<name>] [Append=true|false] [raw] [hex] [all] [chg]\n\t"
    "        <TABLE>[.<COPYNO>] [<INDEX>] [<COUNT>]\n\t"
    "        [-filter <FIELD>=<VALUE>[,...]]\n\t"
    "      If raw is specified, show raw memory words instead of fields.\n\t"
    "      If hex is specified, show hex data only (for Expect parsing).\n\t"
    "      If all is specified, show even empty or invalid entries\n\t"
    "      If chg is specified, show only fields changed from defaults\n\t"
    "      (Use \"listmem\" command to show a list of valid tables)\n\t"
    "DUMP PCIC                     (PCI config space)\n\t"
    "DUMP PCIM [<START> [<COUNT>]] (CMIC PCI registers)\n\t"
    "DUMP SOC [ADDR | RVAL | DIFF] (All SOC registers)\n\t"
    "      ADDR shows only addresses, doesn't actually load.\n\t"
    "      RVAL shows reset defaults, doesn't actually load.\n\t"
    "      DIFF shows only regs not equal to their reset defaults.\n\t"
    "DUMP SOCMEM [DIFF] (All SOC memories)\n\t"
    "      DIFF shows only memories not equal to their reset defaults.\n\t"
    "DUMP MW [<START> [<COUNT>]]   (System memory, 32 bits)\n\t"
    "DUMP MH [<START> [<COUNT>]]   (System memory, 16 bits)\n\t"
    "DUMP MB [<START> [<COUNT>]]   (System memory, 8 bits)\n\t"
    "DUMP SA                       (ARL shadow table)\n\t"
    "DUMP DV ADDR                  (DMA vector)\n\t"
    "DUMP PHY [<PHYID>]            See also, the 'PHY' command.\n"
#endif
    ;
extern cmd_result_t cmd_robo_dump(int unit, args_t *a);

char cmd_robo_eav_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
"Parameters: <cmd> <opt>\n"
#else /* !COMPILER_STRING_CONST_LIMIT */
" Where <cmd> is:\n"
"\tcontrol set|get <ctrl_type> <parameter>\n"
"\tmac get|set <mac value>\n"
"\ttypes. (Describe the types of Ethernet AV CLI commands)\n"
"\tinit\n"
"\tport enable|disable <port number>\n"
"\tlink on|off <port number>\n"
"\tqueue set|get <port number> <queue_control_type> <param>\n"
"\twatch start|stop\n"
"\tstatus\n"
"\ttimestamp <port number>\n"
"\ttimesync set|get <time sync type> <p0> [<p1>]\n"
"\ttx <pbmp> <vlanid>\n"
"\tsrp set|get <mac value> <ethertype>\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
;
extern cmd_result_t cmd_robo_eav(int unit, args_t *a);

#ifdef INCLUDE_EAV_APPL
char cmd_robo_timesync_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
"Parameters: <cmd> <opt>\n"
#else /* !COMPILER_STRING_CONST_LIMIT */
" Where <cmd> is:\n\t"
" timesync start <interval>\n\t"
" timesync stop \n\t"
" timesync port enable|disable|master port\n\t"
" timesync status\n\t"
" timesync debug <port>\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
;
extern cmd_result_t cmd_robo_timesync(int unit, args_t *a);




char cmd_robo_discovery_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
"Parameters: <cmd> <opt>\n"
#else /* !COMPILER_STRING_CONST_LIMIT */
" Where <cmd> is:\n\t"
" discovery start <interval>\n\t"
" discovery stop \n\t"
" discovery forcesend port\n\t"
" discovery status\n\t"
" discovery debug <port>\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
;
extern cmd_result_t cmd_robo_discovery(int unit, args_t *a);


char cmd_robo_bandwidthreserve_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
"Parameters: <cmd> <opt>\n"
#else /* !COMPILER_STRING_CONST_LIMIT */
" Where <cmd> is:\n\t"
" bandwidthreserve start\n\t"
" bandwidthreserve stop\n\t"
" bandwidthreserve status\n\t"
#endif /* !COMPILER_STRING_CONST_LIMIT */
;
extern cmd_result_t cmd_robo_bandwidthreserve(int unit, args_t *a);
#endif

char cmd_robo_pm_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
"Parameters: <cmd> <opt>\n"
#else /* !COMPILER_STRING_CONST_LIMIT */
" Where <cmd> is:\n"
"\tshow (Dispay the current power mode of each port)\n"
"\tset <port bitmap> <mode number>\n"
"\tMode 1 : High Performance Mode\n"
"\tMode 2 : Low Power A0 Mode\n"
"\tMode 3 : Low Power A6 Mode\n"
"\tMode 4 : Green Mode\n"
"\tMode 5 : Auto Power-Down Mode\n"
"\tMode 6 : Standby Power-Down Mode\n"
"\tMode 7 : Disable Standby Power-Down Mode\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
;
extern cmd_result_t cmd_robo_pm(int unit, args_t *a);


char cmd_robo_reg_edit_usage[] =
    "Parameters: <REGISTER>\n\t"
    "<REGISTER> is SOC register symbolic name.\n\t"
    "Loads a register and displays each field, providing an opportunity\n\t"
    "to modify each field.\n";
extern cmd_result_t cmd_robo_reg_edit(int unit, args_t *a);


char if_robo_field_proc_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
"Parameters: <cmd> <opt>\n"
#else /* !COMPILER_STRING_CONST_LIMIT */
" Where <cmd> is:\n"
"\taction <add|get|remove> <eid> [act] [p0] [p1]\n"
"\taction ports <add|get> <eid> <act> [<pbmp>]\n"
"\tcontrol <ctrl_num> [<status>]\n"
"\tcounter create|get|set <eid> [<0|1>] [<val>]\n"
"\tcounter share <src_eid> <dst_eid>\n"
"\tcounter destroy <eid>\n"
"\tdetach\n"
"\tentry create <gid> [<eid>]\n"
"\tentry copy <src_eid> [<dst_eid>]\n"
"\tentry install|reinstall|remove|destroy <eid>\n"
"\tentry prio <eid> [highest|lowest|dontcare|default|<priority>]\n"
"\tgroup create <pri> [<gid>] [mode]\n"
"\tgroup destroy|get|set <gid>\n"
"\tinit\n"
"\trange create [<rid>] [<flags>] [<min>] [<max>]\n"
"\trange group create [<rid>] [<flags>] [<min>] [<max>] [<group>]\n"
"\trange get|destroy <rid>\n"
"\tmeter create|destroy <eid>\n"
"\tmeter share <src_eid> <dst_eid>\n"
"\tmeter getc|getp|setc|getp <eid> [<rate>] [<burst>]\n"
"\tqual <eid> <QUAL> [<udf_id*>] data mask (*required for QUAL=UserDefined)\n"
"\tqset add|clear|show [qualifier]\n"
"\tshow [group|entry] [id]\n"
"\tuser get|set|init|create [udf_id] [flags] [offset]\n"
"\tthread off\n"
"\tstatus\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
;
extern cmd_result_t if_robo_field_proc(int unit, args_t *a);

char cmd_robo_reg_get_usage[] =
    "Parameters: [hex] [<REGTYPE>] <REGISTER|PAGE ADDRESS>\n\t"
    "If <REGTYPE> is not specified, it defaults to \"soc\".\n\t"
    "<REGISTER> is SOC register symbolic name.\n\t"
    "<PAGE> is SOC register page value.\n\t"
    "<ADDRESS> is SOC register address value.\n\t"
    "If hex is specified, dumps only a hex value (for Expect parsing).\n\t"
    "For a list of register types, use \"dump\".\n";

extern cmd_result_t cmd_robo_reg_get(int unit, args_t *a);
char if_robo_igmp_usage[] =
    "IGMP snooping enable/disable\n"
    "With no arguments, displays the IGMP snooping status.\n\t"
    "Usage :\n\t"
    "   igmp  init\n\t"
    "   igmp  on|enable|yes\n\t"
    "   igmp  off|disable|no\n";

extern cmd_result_t if_robo_igmp(int unit, args_t *a);

#ifdef IMP_SW_PROTECT
char if_robo_imp_protect_usage[] =
    "IMP Protect <min rate> <middle rate> <max rate>\n"
    "The granularity of the rate is Kbits per second\n"
    "With no arguments, displays the IMP Protect parameters.\n\t";

extern cmd_result_t if_robo_imp_protect(int unit, args_t *a);

#endif /* IMP_SW_PROTECT */



char if_robo_ipg_usage[] =
    "Parameters: [PortBitMap=<pbmp>] [SPeed=10|100|1000]\n\t"
    "[FullDuplex=true|false]\n\t"
    "Get the IPG register values.\n\t"
    "If no args are given, displays all.\n";

extern cmd_result_t if_robo_ipg(int unit, args_t *a);

char if_robo_l2_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  l2 <option> [args...]\n"
#else    
    "  l2 add [PortBitMap=<pbmp>] [MACaddress=<mac>] [Vlanid=<id>]\n\t"
    "         [PRIority=<cosq>|<pri>] [STatic=true|false]\n\t"
    "         [Replace=true|false]\n\t"
    "        - Add incrementing L2 addresses associated with port(s)\n\t"
    "  l2_replace [Module=<n>] [Port=<port>] [MACaddress=<mac>]\n\t"
    "             [Vlanid=<id>] [Trunk=true|false] [TrunkGroupId=<id>]\n\t"
    "             [STatic=true|false] \n\t"
    "             [NewModule=<n>] [NewPort=<port>] [NewTrunkGroupId=<id>]\n\t"
    "  l2 del [MACaddress=<mac>] [Count=<value>] [Vlanid=<id>]\n\t"
    "        - Delete incrementing L2 address(s)\n\t"
    "  l2 show [PortBitMap=<pbmp]\n\t"
    "        - Show L2 addresses associated with port(s)\n\t"
    "  l2 clear [Port=<port>] [MACaddress=<mac>]\n\t"
    "           [Vlanid=<id>] [TrunkGroupID=<id>] [Static=true|false]\n\t"
    "        - Remove all L2 entries on the given module, module/port,\n\t"
    "           VLAN, or trunk group ID\n\t"    
    "  l2 dump\n\t"
    "        - Dump all entries in L2 table\n\t"
    "  l2 conflict [MACaddress=<mac>] [Vlanid=<id>]\n\t"
    "        - Dump all conflicting L2 entries (same hash bucket)\n\t"
    "  l2 watch [start | stop]\n\t"
    "        - Watch dynamic address insertions/deletions\n\t"
    "  l2 cache add [CacheIndex=<index>] [MACaddress=<mac>]\n\t"
    "           [PortBitMap=<pbmp>] [DestPorts==true|false]\n\t"
    "        - Add L2 cache entry\n\t"
    "  l2 cache del CacheIndex=<index> [Count=<value>]\n\t"
    "        - Delete L2 cache entry\n\t"
    "  l2 cache show\n\t"
    "        - Show L2 cache entries\n\t"
    "  l2 cache clear\n\t"
    "        - Delete all L2 cache entries\n"
#endif
    ;

extern cmd_result_t if_robo_l2(int unit, args_t *a);

char if_robo_linkscan_usage[] =
    "Parameters: [SwPortBitMap=<pbmp>] [HwPortBitMap=<pbmp>]\n\t"
    "[Interval=<usec>] [FORCE=<pbmp>]\n"
#ifndef COMPILER_STRING_CONST_LIMIT    
    "\tWith no arguments, displays the linkscan status for all ports.\n\t"
    "Enables software linkscan on ports specified by SwPortBitMap.\n\t"
    "Enables hardware linkscan on ports specified by HwPortBitMap.\n\t"
    "Disables linkscan on all other ports.\n\t"
    "Interval specifies non-default linkscan interval for software.\n\t"
    "Note: With linkscan disabled, autonegotiated ports will NOT have\n\t"
    "the MACs updated with speed/duplex..\n\t"
    "FORCE=<pbmp> requests linkscan to update ports once even if link\n\t"
    "status did not change.\n"
#endif
    ;
extern cmd_result_t if_robo_linkscan(int unit, args_t *a);

char cmd_robo_mem_list_usage[] =
    "Parameters: <TABLE>\n\t"
    "If no parameters are given, displays a reference list of all\n\t"
    "memories and their attributes.\n\t"
    "If TABLE is given, displays the entry fields for that table.\n";

extern cmd_result_t cmd_robo_mem_list(int unit, args_t *a);

char cmd_robo_reg_list_usage[] =
    "1. Parameters: <REGISTER|PAGE ADDRESS> [<VALUE>]\n\t"
    "Lists all register fields in <REGISTER>\n\t"
    "<REGISTER> is SOC register symbolic name.\n\t"
    "<PAGE> is SOC register page value.\n\t"
    "<ADDRESS> is SOC register address value.\n\t"
    "2. Parameters: <SUBSTRING>\n\t"
    "Prints all SOC register names containing substring.\n\t"
    "3. Parameters: *\n\t"
    "Prints all SOC registers.\n";
extern cmd_result_t cmd_robo_reg_list(int unit, args_t *a);

char if_robo_mcast_usage[] =
    "Usages:\n\t"
    "  mcast add <MACaddress>=<val> <Vlanid>=<val> <PortBitMap>=<val>\n\t"
    "                  <Index>=<val>\n\t"
    "  mcast delete <MACaddress>=<val> <Vlanid>=<val>\n\t"
    "  mcast join <MACaddress>=<val> <Vlanid>=<val> <PortBitMap>=<val>\n\t"
    "  mcast leave <MACaddress>=<val> <Vlanid>=<val> <PortBitMap>=<val>\n\t"
    "  mcast padd <MACaddress>=<val> <Vlanid>=<val> <PortBitMap>=<val>\n\t"
    "  mcast premove <MACaddress>=<val> <Vlanid>=<val> <PortBitMap>=<val>\n\t"
    "  mcast pget <MACaddress>=<val> <Vlanid>=<val>\n";

extern cmd_result_t if_robo_mcast(int unit, args_t *a);

char if_robo_mirror_usage[] =
    "Usages:\n"
    "  mirror Mode=<L2|Off>        -Mirror L2 or none\n"
    "  mirror Port=<port>          -Set the mirror destination port\n"
    "  mirror IngressBitMap=<pbmp> -PortBitMap with ingress mirroring enabled\n"
    "  mirror EgressBitMap=<pbmp>  -PortBitMap with egress mirroring enabled\n";
extern cmd_result_t if_robo_mirror(int unit, args_t *a);


char cmd_robo_mem_modify_usage[] =
    "Parameters: <TABLE> <ENTRY> <ENTRYCOUNT>\n\t"
    "        <FIELD>=<VALUE>[,...]\n\t"
    "Read/modify/write field(s) of a table entry(s).\n";

extern cmd_result_t cmd_robo_mem_modify(int unit, args_t *a);

char cmd_robo_reg_mod_usage[] =
    "Parameters: <REGISTER> <FIELD>=<VALUE>[,...]\n\t"
    "<REGISTER> is SOC register symbolic name.\n\t"
    "<FIELD>=<VALUE>[,...] is a list of fields to affect,\n\t"
    "Fields not specified in the list are left unaffected.\n";
extern cmd_result_t cmd_robo_reg_mod(int unit, args_t *a);

char cmd_robo_pbmp_usage[] =
    "Parameters: <pbmp>\n"
#ifndef COMPILER_STRING_CONST_LIMIT    
    "Converts a pbmp string into a hardware port bitmap.  A pbmp string\n\t"
    "is a single port, or a group of ports specified in a list using ','\n\t"
    "to separate them and '-' for ranges, e.g. 1-8,25,cpu.  Ports may be\n\t"
    "specified using the one-based port number (1-29) or port type and\n\t"
    "zero-based number (fe0-fe23,ge0-ge7).  'cpu' is equal to port 24,\n\t"
    "'fe' is all FE ports, 'ge' is all GE ports, 'e' is all ethernet\n\t"
    "ports, 'all' is all ports, and 'none' is no ports (0x0).\n\t"
    "A '~' may be used to exclude port previously given (e.g. e,~fe19)\n\t"
    "Acceptable strings and values also depend on the chip being used.\n\t"
    "A pbmp may also be given as a raw hex (0x) number, e.g. 0xbffffff.\n"
#endif
    ;
extern cmd_result_t cmd_robo_pbmp(int unit, args_t *a);
                     
char if_robo_phy_usage[] =
    "Parameters: [int] <ports> <regnum> [<value>]\n\t"
    "                 raw <mii-addr> <regnum> [<value>]\n"
#ifndef COMPILER_STRING_CONST_LIMIT    
    "\tSet or display PHY registers.  If only <ports> is specified,\n\t"
    "then registers for those ports' PHYs are displayed. <ports> is a\n\t"
    "standard port bitmap, e.g. fe for all 10/100 ports, fe5-fe7 for\n\t"
    "three FE's, etc. (see \"help pbmp\").  If the int option is given,\n\t"
    "the intermediate PHY for the port is used instead of the outer PHY.\n\t"
    "In 'raw' mode, the direct mii-address can be specified, only\n\t"
    "'writing' is supported.\n"
#endif
    ;
extern cmd_result_t if_robo_phy(int unit, args_t *a);

char if_robo_port_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: port <option> [args...]\n"
#else    
    "Parameters: <ports> [AutoNeg=on|off] [enable] [disable]\n\t"
    "[STP=Disable|Block|LIsten|LEarn|Forward] [detach] [probe] [attach]\n\t"
    "[LinkScan=on|off|hw|sw] [SPeed=10|100|1000] [FullDuplex=true|false]\n\t"
    "[TxPAUse=on|off RxPAUse=on|off] [DISCard=none|untag|all]\n\t"
    "[PRIOrity=<0-7>] [PHymaster=<Master|Slave|Auto>]\n\t"
    "If only <ports> is specified, characteristics for that port are\n\t"
    "displayed. <ports> is a standard port bitmap (see \"help pbmp\").\n\t"
    "If AutoNeg is on, SPeed and DUPlex are the ADVERTISED MAX values.\n\t"
    "If AutoNeg is off, SPeed and DUPlex are the FORCED values.\n\t"
    "SPeed of zero indicates maximum speed.\n\t"
    "LinkScan enables automatic scanning for link changes with updating\n\t"
    "of MAC registers.\n\t"
    "PAUse enables send/receive of pause frames in full duplex mode.\n\t"
    "PRIOrity sets the priority for untagged packets coming on this port.\n"
#endif
    ;
extern cmd_result_t if_robo_port(int unit, args_t *a);

char if_robo_port_rate_usage[] =
    "Set/Display port rate metering characteristics.\n"
    "Parameters: <pbm> [ingress|egress|queue_egress [arg1 arg2 <arg3>]]\n\t"
    "    If no parameters, show status of all ports\n\t"
    "    For Ingress or Egress: arg1 is rate, arg2 is max_burst\n\t"
    "    For Queue_Egress: arg1 is queue id, arg2 is rate, arg3 is max_burst, \n\t"
    "    rate is in kilobits (1000 bits) per second\n\t"
    "    max_burst are in kilobits (1000 bits)\n";
extern cmd_result_t if_robo_port_rate(int unit, args_t *a);

char if_robo_port_stat_usage[] =
    "Display info about port status in table format.\n"
    "    Link scan modes:\n"
    "        SW = software\n"
    "        HW = hardware\n"
    "    Learn operations (source lookup failure control):\n"
    "        F = SLF packets are forwarded\n"
    "        C = SLF packets are sent to the CPU\n"
    "        A = SLF packets are learned in L2 table\n"
    "    Pause:\n"
    "        TX = Switch will transmit pause packets\n"
    "        RX = Switch will obey pause packets\n";
extern cmd_result_t if_robo_port_stat(int unit, args_t *a);

char cmd_robo_reg_set_usage[] =
    "1. Parameters: [<REGTYPE>] <REGISTER|arg1 arg2> <VALUE>\n\t"
    "\tIf <REGTYPE> is not specified, it defaults to \"soc\".\n\t"
    "\t<REGISTER> is SOC register symbolic name.\n\t"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tIf <REGTYPE> is SOC - <arg1> is SOC register page value.\n\t"
    "\t                      <arg2> is SOC register address value.\n\t"
    "\tIf <REGTYPE> is PHY - <arg1> is phy address value.\n\t"
    "\t                      <arg2> is register offset value.\n\t"
#endif
    "2. Parameters: <REGISTER> <FIELD>=<VALUE>[,...]\n\t"
    "\t<REGISTER> designates symbolic name.\n\t"
    "\t<FIELD>=<VALUE>[,...] is a list of fields to affect,\n\t"
    "\tfor example: L3_ENA=0,CPU_PRI=1.\n\t"
    "\tFields not specified in the list are set to zero.\n\t"
    "For a list of register types, use \"help dump\".\n";
extern cmd_result_t cmd_robo_reg_set(int unit, args_t *a);

char if_robo_pvlan_usage[] =
    "Usages:\n\t"
    "  pvlan show <pbmp>\n\t"
    "        - Show PVLAN info for these ports.\n\t"
    "  pvlan set <pbmp> <vid>\n\t"
    "        - Set default VLAN tag for port(s)\n\t"
    "          Port bitmaps are read from the VTABLE entry for the VID.\n\t"
    "          <vid> must have been created and all ports in <pbmp> must\n\t"
    "          belong to that VLAN.\n";
extern cmd_result_t if_robo_pvlan(int unit, args_t *a);

char cmd_robo_rate_usage[] =
    "Parameters:[PortBitMap=<pbm>] [Limit=<limit>]\n\t"
    "[Bcast=<1|0>] [Mcast=<1|0>] [Dlf=<1|0>]\n\t"
    "Enables the specified packet rate controls.\n\t"
    "  pbm       port(s) to set up or display\n\t"
    "  Limit     kilo bits per second\n\t"
    "  Bcast     Enable broadcast rate control\n\t"
    "  Mcast     Enable multicast rate control\n\t"
    "  Dlf       Enable DLF flooding rate control\n\t"
    "<limit> = 0, Bcast/Mcast/Dlf = 1: Disable Bcast/Mcast/Dlf suppression\n\t"
    "<limit> > 0, Bcast/Mcast/Dlf = 1: Enable Bcast/Mcast/Dlf suppression\n";
extern cmd_result_t cmd_robo_rate(int unit, args_t *a);

char cmd_robo_rx_cfg_usage[] =
    "rxcfg [<chan>] [options...]\n"
#ifndef COMPILER_STRING_CONST_LIMIT    
    "    With no options, displays current configuration\n"
    "    Global options include:\n"
    "        SPPS=<n>            Set system-wide packet per second limit\n"
    "                            Other options combined this are ignored\n"
    "        GPPS=<n>            Set global packet per second limit\n"
    "        PKTSIZE=<n>         Set maximum receive packet size\n"
    "        PPC=<n>             Set the number of pkts/chain\n"
    "        BURST=<n>           Set global packet burst size\n"
    "        COSPPS<n>=<r>       Set the per COS rate limiting\n"
    "        FREE=[T|F]          Should handler free buffers?\n"
    "    Channel specific options include:\n"
    "        CHAINS=<n>          Set the number of chains for the channel\n"
    "        PPS=<n>             Set packet per second for channel\n"
    "    Global options can be given w/o a channel.  Channel options\n"
    "    require that a channel be specified.\n"
    "    The channel's burst rate is #chains * pkts/chain\n"
#endif
    ;
extern cmd_result_t cmd_robo_rx_cfg(int unit, args_t *a);

char cmd_robo_rx_init_usage[] =
    "RXInit <override-unit>\n"
    "    Call bcm_rx_init on the given override unit. Ignores\n"
    "    the current unit.\n";
extern cmd_result_t cmd_robo_rx_init(int unit, args_t *a);

char cmd_robo_rx_mon_usage[] =
    "Parameters [init|start|stop|show]\n"
#ifndef COMPILER_STRING_CONST_LIMIT    
    "With no parameters, show whether or not active.\n"
    "    init:    Initialize the RX API, but don't register handler\n"
    "    start:   Call RX start with local pkt dump routine\n"
    "             Modify the configuration with the rxcfg command\n"
    "    stop:    Call RX stop\n"
    "    show:    Call RX show\n"
#endif
    ;
extern cmd_result_t cmd_robo_rx_mon(int unit, args_t *a);

char cmd_robo_show_usage[] =
    "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  show <args>\n"
#else
/*
    "  show Pci        - Probe and display function 0 of all busses/devices\n"
*/
    "  show CHips      - Show all driver-supported device IDs\n"
    "  show Counters [Changed] [Same] [Z] [NZ] [Hex] [Raw] [<reg> | <pbmp>]\n"
    "\tDisplay all counters, or only specified regs and/or ports\n"
    "\t  Changed - include counters that changed\n"
    "\t  Same    - include counters that did not change\n"
    "\t  Z       - include counters that are zero\n"
    "\t  Nz      - include counters that are not zero\n"
    "\t  All     - same as: Changed Same Z Nz\n"
    "\t  Hex     - display counters as 64-bit hex values\n"
    "\t  Raw     - display raw 64-bit hex values, no register name(s)\n"
    "\t  ErDisc  - Only show those counters marked with Error/Discard\n"
    "\tNOTES: If neither Changed or Same is specified, Change is used.\n"
    "\t       If neither Z or Nz is specified, Nz is used.\n"
    "\t       Use All to display counters regardless of value.\n"
    "  show Statistics [pbm] [all] - SNMP accumulated statistics,all shows 0s\n"
    "  show Errors            - logged error counts for certain errors\n"
    "  show params [<chip>]   - Chip parameters (chip id or current unit)\n"
    "  show unit [<unit>]     - Unit list or unit parameters\n"
    "  show features [all]    - Show enabled (or all) features for this unit\n"
#if defined(VXWORKS)
    "  show ip                - IP statistics\n"
    "  show icmp              - ICMP statistics\n"
    "  show arp               - ARP statistics\n"
    "  show udp               - UDP statistics\n"
    "  show tcp               - TCP statistics\n"
    "  show mux               - MUX protocol stack backplane\n"
    "  show routes            - IP routing table\n"
    "  show hosts             - IP host table\n"
#endif /* VXWORKS */
#endif
    ;
extern cmd_result_t cmd_robo_show(int unit, args_t *a);


char cmd_robo_soc_usage[] =
    "Parameters: [<unit #>] ... \n\t"
    "Print internal SOC driver control information IF compiled as a \n\t"
    "debug version. If not compiled as a debug version, a warning is\n\t"
    "printed and the command completes successfully with no further\n\t"
    "output\n";
extern cmd_result_t cmd_robo_soc(int unit, args_t *a);

char if_robo_stg_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  stg <option> [args...]\n"
#else    
    "  stg create [<id>]            - Create a STG; optionally specify ID\n\t"
    "  stg destroy <id>             - Destroy a STG\n\t"
    "  stg show [<id>]              - List STG(s)\n\t"
    "  stg add <id> <vlan_id> [...]     - Add VLAN(s) to a STG\n\t"
    "  stg remove <id> <vlan_id> [...]  - Remove VLAN(s) from a STG\n\t"
    "  stg clear <id>          - Remove ALL VLAN(s) from a STG\n\t"
    "  stg stp                      - Get span tree state, all ports/STGs\n\t"
    "  stg stp <id>                 - Get span tree state of ports in STG\n\t"
    "  stg stp <id> <pbmp> <state>  - Set span tree state of ports in STG\n\t"
    "                                 (disable/block/listen/learn/forward)\n\t"
    "  stg default [<id>]           - Show or set the default STG\n"
#endif
    ;
extern cmd_result_t if_robo_stg(int unit, args_t *a);

char if_robo_trunk_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  trunk <option> [args...]\n"
#else    
    "  trunk init\n\t"
    "        - Initialize trunking function\n\t"
    "  trunk deinit\n\t"
    "        - Deinitialize trunking function\n\t"
    "  trunk add <Id=val> <Rtag=val> <Pbmp=val>\n\t"
    "        - Add ports to a trunk\n\t"
    "  trunk remove <Id=val> <Pbmp=val>\n\t"
    "        - Remove ports from a trunk\n\t"
    "  trunk show [<Id=val>]\n\t"
    "        - Display trunk information\n\t"
    "  trunk psc <Id=val> <Rtag=val>\n\t"
    "        - Change Rtag(for testing ONLY)\n\t"
    "  trunk mcast <Id=val> <Mac=val> <Vlan=val>\n\t"
    "        - Join multicast to a trunk\n"
#endif
    ;
extern cmd_result_t if_robo_trunk(int unit, args_t *a);

char cmd_robo_tx_usage[] =
    "Parameters: <Count> [options]\n"
#ifndef COMPILER_STRING_CONST_LIMIT    
    "  Transmit the specified number of packets, if the contents of the\n"
    "  packets is important, they may be specified using options.\n"
    "  Supported options are:\n"
    "      PortBitMap=<pbmp>   - Specify port bitmap packet is sent to.\n"
    "      UntagBitMap=<pbmp>  - Specify untag bitmap used for DMA.\n"
    "      Length=<value>      - Specify the total length of the packet,\n"
    "                            including header, possible tag, and CRC.\n"
    "      VLantag=<value>     - Specify the VLAN tag used, only the low\n"
    "                            order 16-bits are used (VLANID=0 for none)\n"
    "      Pattern=<value>     - Specify 32-bit data pattern used.\n"
    "      PatternInc=<value>  - Value by which each word of the data\n"
    "                            pattern is incremented\n"
    "      PerPortSrcMac=[0|1] - Associate specific (different) src macs\n"
    "                            with each source port.\n"
    "      SourceMac=<value>   - Source MAC address in packet\n"
    "      SourceMacInc=<val>  - Source MAC increment\n"
    "      DestMac=<value>     - Destination MAC address in packet.\n"
    "      DestMacInc=<vale>   - Destination MAC increment.\n"
#endif
    ;

extern cmd_result_t cmd_robo_tx(int unit, args_t *a);


char cmd_robo_tx_count_usage[] =
    "Parameters: None\n\t"
    "Print current request count and set count values for an active\n\t"
    "TXSTART command.\n";
extern cmd_result_t cmd_robo_tx_count(int unit, args_t *a);
extern cmd_result_t cmd_robo_tx_start(int unit, args_t *a);

char cmd_robo_tx_stop_usage[] =
    "Parameters: None\n\t"
    "Terminate a background TXSTART command that is currently running.\n\t"
    "This command only requests termination, the background thread\n\t"
    "looks for termination requests BETWEEN sending packets.\n";
extern cmd_result_t cmd_robo_tx_stop(int unit, args_t *a);

char if_robo_vlan_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  vlan <option> [args...]\n"
#else    
    "  vlan create <id> [PortBitMap=<pbmp> UntagBitMap=<pbmp>]\n\t"
    "                                       - Create a VLAN\n\t"
    "  vlan destroy <id>                    - Destroy a VLAN\n\t"
    "  vlan clear                           - Destroy all VLANs\n\t"
    "  vlan add <id> [PortBitMap=<pbmp> UntagBitMap=<pbmp>\n\t"
    "                                       - Add port(s) to a VLAN\n\t"
    "  vlan remove <id> [PortBitMap=<pbmp>] - Remove ports from a VLAN\n\t"
    "  vlan show                            - Display all VLANs\n\t"
    "  vlan default [<id>]                  - Show or set the default VLAN\n\t"
#ifdef BCM_PROTOCOL2V_SUPPORT
    "  vlan protocol [enable|disable] PortBitMap=<pbmp>\n\t"
    "  vlan protocol add Ether=<N> VLan=<vlanid>\n\t"
    "  vlan protocol delete Ether=<N>\n\t"
    "  vlan protocol clear \n\t"
#endif  /* BCM_PROTOCOL2V_SUPPORT */
#ifdef BCM_MAC2V_SUPPORT
    "  vlan mac [enable|disable] PortBitMap=<pbmp>\n\t"
    "  vlan mac add MACaddress=<address> VLan=<vlanid> Prio=<prio>\n\t"
    "  vlan mac delete MACaddress=<address>\n\t"
    "  vlan mac clear\n\t"
    "  vlan mac show MACaddress=<address>\n\t"
#endif  /* BCM_MAC2V_SUPPORT */
#ifdef BCM_V2V_SUPPORT
    "  vlan translate [enable|disable] PortBitMap=<pbmp>\n\t"
    "  vlan translate add Port=<port> OldVLan=<vlanid> NewVLan=<vlanid> dtag=<T|F>\n\t"
    "  vlan translate get Port=<port> OldVLan=<vlanid> \n\t"
    "  vlan translate delete Port=<port> OldVLan=<vlanid>\n\t"
    "  vlan translate clear\n\t"
    "  vlan translate egress add Port=<port> OldVLan=<vlanid> NewVLan=<vlanid>\n\t"
    "        Prio=<prio> Cng=<cng>\n\t"
    "  vlan translate egress get Port=<port> OldVLan=<vlanid> \n\t"
    "  vlan translate egress delete Port=<port> OldVLan=<vlanid>\n\t"
    "  vlan translate egress clear\n\t"
    "  vlan translate dtag add Port=<port> OldVLan=<vlanid> NewVLan=<vlanid>\n\t"
    "        Prio=<prio> Cng=<cng>\n\t"
    "  vlan translate dtag get Port=<port> OldVLan=<vlanid> \n\t"
    "  vlan translate dtag delete Port=<port> OldVLan=<vlanid>\n\t"
    "  vlan translate dtag clear\n\t"
#endif  /* BCM_V2V_SUPPORT */
    "  vlan control <name> <value>\n\t"
#endif
    "  vlan <id> <name>=<vlaue>         - Set/Get per VLAN property\n"
    ;
extern cmd_result_t if_robo_vlan(int unit, args_t *a);


char cmd_robo_mem_write_usage[] =
    "Parameters: <TABLE> <ENTRY> <ENTRYCOUNT>\n\t"
    "        { <DW0> .. <DWN> | <FIELD>=<VALUE>[,...] }\n\t"
    "Number of <DW> must be a multiple of table entry size.\n\t"
    "Writes entry(s) into table index(es).\n";
extern cmd_result_t cmd_robo_mem_write(int unit, args_t *a);

