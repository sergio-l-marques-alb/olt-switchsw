package require ase_misc_utils
package require SqaCommon          2.0
package require SqaApiUtils        2.0
package require SqaCmdLog          2.0
package require SqaCmdLib          2.0
package require SqaDeviceUtils     2.0
package require SqaDeviceConfig    2.0
package require SqaResetOnFail     2.0
package require SqaFeatureSupport  2.0
package require SqaExecMode        2.0
package require SqaFileUtils       2.0

# next version libs
package require SqaTrillLib    2.0
package require SqaPortLib     2.0
package require SqaRtag7Lib    2.0
package require SqaTrunkLib    2.0
package require SqaMcastLib    2.0
package require SqaVlanLib     2.0
package require SqaL3Lib       2.0
package require SqaL2Lib       2.0
package require SqaSwitchLib   2.0
package require SqaMplsLib     2.0
package require SqaSerLib      2.0
package require SqaSocLib      2.0
package require SqaStatLib     2.0
package require SqaPktData     2.0
package require SqaPktTxRx     2.0
package require warmBoot
#SER warm boot testcase
#
# @ Traffic Tested:
#
# @Purpose:
#    Test to verify WARMBOOT SER handling of memory=MPLS_ENTRY
#
# @Steps:
#    1. Create Multiple entries
#    2. Introduce error in one of the entries
#    3. Send packet to hit the errored entry
#    4. Verify packet fowarding is OK after errored entry restored.
#    5. Introduce the error again, then warmboot
#    6. After warmboot, use api to check
#    7. Introduce errors in two of the entries
#    8. Send packet to hit the errored entries
#    9. Verify packet fowarding is OK after errored entries restored.
#    10. Clean the configuration
proc AT_SER_Tr_MplsEntry {chip unit setup check cleanup} {

    FEATURE_SUPPORT $unit {MPLS}
    FEATURE_SUPPORT $unit {SER}

    # Switch event map
    SwitchEventMap $unit
    ## DEBUG
    LogLevelSet Silent
    #Enable soc property force_read_through
    SocControlSet $unit {FORCE_READ_THROUGH}  $::TRUE


    if {$setup} {
        set mem         MPLS_ENTRY
        set memVal      $::MPLS_ENTRYm

        # vars
        set p_cnt       2
        set count       10
        set vid         [VlanRandIdGet $unit 1]
        set dst_mac     00:00:00:00:01:01
        set nh_mac      00:01:01:01:01:01
        set rtr_mac     00:AA:AA:AA:AA:01
        set term_lbl    [RandInt 20]
        set egr_lbl     [RandInt 20]
        set flg         0
        set act         $::BCM_MPLS_SWITCH_ACTION_SWAP
        set pipe_inject -1

        set pL [PortSelect $unit $p_cnt {-e} -nomodidset -tx_ready -no_remove_ports -no_remove_cpu]
        LogTest "Selected ports=($pL)"
        ListVarsAssign $pL {port1 port2}
        set gL [PortGportGet $unit $pL]
        ListVarsAssign $gL {gport1 gport2}

        #select pipe to inject error if mutipipe is supported
        if {[ChipModVarGet $unit {DEV} multi_pipe_support]} {
            set pipe_inject [PortListPipeGet $unit $gport1]
        }
        LogTest "pipe_inject = $pipe_inject"
        LogTest "Create Vlan"
        VlanCreatePortAdd $unit $vid $pL

        LogTest "Create $count Egress L3 Intf"
        set intfL ""
        for {set cntr 1} {$cntr <= $count} {incr cntr} {
            set intf [L3IntfCreate $unit mac_addr $rtr_mac vid $vid flags $::BCM_L3_ADD_TO_ARL]
            set rtr_mac  [MacOpr $rtr_mac {+} 2]
            lappend intfL $intf
        }

        LogTest "Create $count L3 Egress Objects"
        foreach intfId $intfL {
            set nhIdx [L3EgressCreate $unit mac_addr $nh_mac vlan $vid  port $port2 intf $intfId mpls_label $egr_lbl]
            incr egr_lbl
            set nh_mac  [MacOpr $nh_mac {+} 2]
            lappend l3ObjL $nhIdx
        }

        LogTest "Create Tunnel switch entries"
        foreach l3_obj_id $l3ObjL {
            MplsTunnelSwitchAdd $unit \
                label     $term_lbl  \
                egress_if $l3_obj_id \
                action    $act       \
                flags     $flg
            lappend Lable_list $term_lbl
            set L3Obj($term_lbl) $l3_obj_id
            incr term_lbl
        }

        LogTest "Get entry hash index for the table"
        foreach label $Lable_list {
           set key "$label 0"
           set hwIdx($label) [SocTableIdxGet $unit $mem {MPLS} $key]
        }

        # Install L2 tunnel MAC
        set dmacHW [MacFormat {TO-HW} $dst_mac]
        CMD_E_NONE {bcm_l2_tunnel_add $unit $dmacHW $vid}

        foreach label $Lable_list {
          set data [PktDataBuild {MplsL2} -lblCount {1} -mplsDest $dst_mac -mplsVid $vid -mplsLbl_1 $label]
          PktTxReady $unit PKT $data port $port1
          set pkt($label) $PKT
          lappend PKTLIST $PKT
        }

        LogTest "Send packets"
        SnmpStatClear $unit $pL
        PktTxList $unit $PKTLIST
        StatSnmpCheckClear $unit $port1 IfInUcastPkts $count $port2 IfOutUcastPkts $count
        set Lable_list_save $Lable_list
        set label [RandSelectConsume Lable_list]
        set errIdx $hwIdx($label)
        LogTest "Introduce Error at index=($errIdx)"
        SerErrorInjectIndex $unit 0 $memVal $errIdx $pipe_inject

        set regMemInfo "-matchData {MEM $memVal $errIdx}"
        SerEventHandler $unit $::TRUE $regMemInfo

        LogTest "Send packet to hit the entry at index=($errIdx)"
        PktTx $unit $pkt($label)
        SerProcessEvent $unit $::TRUE $regMemInfo
        MplsTunnelSwitchCmp $unit label $label egress_if $L3Obj($label) action $act flags $flg

        SnmpStatClear $unit $pL
        PktTxList $unit $PKTLIST
        StatSnmpCheckClear $unit $port1 IfInUcastPkts $count $port2 IfOutUcastPkts $count

        # if both check and setup equal 1, means normal SER case, otherwise means warmboot case
        if {!$check} {
            #In setup
            #packet structure cannot save, so free and unreset
            PktMemFree $unit $PKTLIST
            unset PKTLIST
            unset data
            unset PKT
            unset pkt
            # Get the current tunnel structure of the index to save
            LogTest "Get the tunnel switch entry before inject error"
            MplsTunnelSwitchGet $unit tnlGet $label "" $flg
            #display_structure $tnlGet
            ChangeStructToList $tnlGet tnlGetItemList tnlGetValueList
            #inject an error to that entry
            #it should be recover before system scrumb(before WB)
            LogPuts "Introduce Error at index=($errIdx)again before warmboot"
            SerErrorInjectIndex $unit 0 $memVal $errIdx $pipe_inject
        }
        SaveTestVars
    }
    LoadTestVars
    if {$check} {
        if {!$setup} {
            #In check case:
            #It should be already recovered the SER error
            LogTest "Get the result after warmboot"
            MplsTunnelSwitchGet $unit tnlGetWB $label "" $flg
            #display_structure $tnlGetWB
            ChangeStructToList $tnlGetWB tnlGetItemWBList tnlGetValueWBList
            LogPuts "Before WB tnlGetValueList is  $tnlGetValueList"
            LogPuts "After WB tnlGetValueWBList is $tnlGetValueWBList"
            LogTest "Compare the results"
            foreach tnlGet $tnlGetValueList tnlGetWB $tnlGetValueWBList tnlGetItem $tnlGetItemList {
                NTest::AssertEq $tnlGet $tnlGetWB  FAIL "Tunnel switch entry Mismatch after warmboot \
                in $tnlGetItem of bcm_mpls_tunnel_switch_t, before WB is $tnlGet, after WB is $tnlGetWB"
            }
            #need rebuild the packets, as strucuture cannot save in setup
            foreach label $Lable_list_save {
               set data [PktDataBuild {MplsL2} -lblCount {1} -mplsDest $dst_mac -mplsVid $vid -mplsLbl_1 $label]
               PktTxReady $unit PKT $data port $port1
               lappend PKTLIST $PKT
            }
        }

        set label1 [RandSelectConsume Lable_list]
        set errIdx1 $hwIdx($label1)
        set label2 [RandSelectConsume Lable_list]
        set errIdx2 $hwIdx($label2)
        LogTest "Introduce Error at index=($errIdx1,$errIdx2)"
        SerErrorInjectIndex $unit 0 $memVal $errIdx1 $pipe_inject
        SerErrorInjectIndex $unit 0 $memVal $errIdx2 $pipe_inject

        LogTest "Send packets to hit the entries at index=($errIdx1,$errIdx2)"
        PktTx $unit $PKTLIST
        StatSnmpCheckClear $unit $port2 IfOutUcastPkts [expr $count - 2]
        MplsTunnelSwitchCmp $unit label $label1 egress_if $L3Obj($label1) action $act flags $flg
        MplsTunnelSwitchCmp $unit label $label2 egress_if $L3Obj($label2) action $act flags $flg

        SnmpStatClear $unit $pL
        PktTxList $unit $PKTLIST
        StatSnmpCheckClear $unit $port1 IfInUcastPkts $count $port2 IfOutUcastPkts $count
    }

    if {$cleanup} {
        LogTest "Clean up"
        PktMemFree $unit $PKTLIST
        MplsConfigReset $unit
        L3ConfigReset $unit
        L2ConfigReset $unit
        PortConfigReset $unit $pL
    }
    return PASS

}

#warmboot setup test
proc WarmBootNS::AT_ser_wb_M_001_Setup {chip unit} {
    #NTestSeedInit need within the test and not in a library
    NTestSeedInit
    AT_SER_Tr_MplsEntry $chip $unit 1 0 0
    return PASS
}

#warmboot check test
proc WarmBootNS::AT_ser_wb_M_001_Check {chip unit} {
    AT_SER_Tr_MplsEntry $chip $unit 0 1 1
    return PASS
}

#
# @ Traffic Tested:
#
# @Purpose:
#    Test to verify WARMBOOT SER handling for different view of 
#    L3_ENTRY, the test verify the view of L3_ENTRY_IPV4_UNICAST
#    L3_ENTRY_IPV6_UNICAST L3_ENTRY_IPV4_MULTICAST and 
#    L3_ENTRY_IPV6_MULTICAST
# @Steps:
#    1. Create Multiple entries for each view
#    2. Introduce error for each entry
#    3. Send packet to hit the errored entry
#    4. Verify packet fowarding is OK after errored entry restored.
#    5. Introduce the error again, then warmboot
#    6. After warmboot, use api to check
#    7. Introduce errors in two of the entries
#    8. Send packet to hit the errored entries
#    9. Verify packet fowarding is OK after errored entries restored.
#    10. Clean the configuration
proc AT_SER_Tr_L3Entry {chip unit setup check cleanup} {
    
    FEATURE_SUPPORT $unit {SER}

    # Switch event map
    SwitchEventMap $unit
    ## DEBUG
    LogLevelSet Silent
    #Enable soc property force_read_through
    SocControlSet $unit {FORCE_READ_THROUGH}  $::TRUE
    if {$setup} {
       set mem1           L3_ENTRY_IPV4_UNICAST
       set mem1Val        $::L3_ENTRY_IPV4_UNICASTm 
       set mem2           L3_ENTRY_IPV6_UNICAST
       set mem2Val        $::L3_ENTRY_IPV6_UNICASTm
       set mem3           L3_ENTRY_IPV4_MULTICAST
       set mem3Val        $::L3_ENTRY_IPV4_MULTICASTm
       set mem4           L3_ENTRY_IPV6_MULTICAST
       set mem4Val        $::L3_ENTRY_IPV6_MULTICASTm
       
       set intfMac1      00:00:00:00:00:11
       set intfMac2      00:00:00:00:00:12
       set nhMac         00:00:00:00:00:13
       set smac          00:00:00:00:00:14
       set vrf           0
       set v_cnt         2
       set p_cnt         2
       set count         5 
       set pipe_inject   -1
       set ip4Addr1      [Ip4Rand]
       set ip4Addr2      [Ip4Opr $ip4Addr1 {+} 100]    ;#used for mcast entries
       set sip4          1.1.1.1
       set mcip4Addr     225.1.1.1
       set McastMac4     01:00:5e:00:00:01         
       set ip6Addr1      [Ip6Rand]
       set ip6Addr2      [Ip6Opr $ip6Addr1 {+} 100]    ;#used for mcast entries
       set sip6          12CD:0000:0000:0000:0000:0000:0000:0001
       set mcip6Addr     FF01:0000:0000:0000:0000:0000:0000:0102
       set McastMac6     33:33:00:00:01:02
        
       LogTest "Select $p_cnt Ports"    
       set pL [PortSelect $unit $p_cnt {-e} -nomodidset -tx_ready -no_remove_ports -no_remove_cpu]
       LogTest "Selected ports=($pL)"
       ListVarsAssign $pL {port1 port2}
       set gL [PortGportGet $unit $pL]
       ListVarsAssign $gL {gport1 gport2}
     
       if {[ChipModVarGet $unit {DEV} multi_pipe_support]} {
          set pipe_inject [PortListPipeGet $unit $gport1]
       }
        
       LogTest "Create Vlan"
       set vidL [VlanRandIdGet $unit $v_cnt]
       ListVarsAssign $vidL {vid1 vid2}
       VlanCreatePortAdd $unit $vidL "$port1 $port2" 1
       
       LogTest "Create L3 Interfaces"
       set intfL [L3IntfIdRandGet $unit $v_cnt]
       ListVarsAssign $intfL {intf1 intf2}
       set intfId1 [L3IntfCreate $unit mac_addr $intfMac1 vid $vid1 intf_id $intf1 flags $::BCM_L3_ADD_TO_ARL]
       set intfId2 [L3IntfCreate $unit mac_addr $intfMac2 vid $vid2 intf_id $intf2 flags $::BCM_L3_ADD_TO_ARL]
       
       LogTest "Create L3_ENTRY_IPV4_UNICAST entries"
       set host4L ""
       for {set cntr 0} {$cntr < $count} {incr cntr} {
           LogTest "Create Egress Object"
           set nhIdx [L3EgressCreate $unit $nhMac $vid2 $port2 $intfId2 -1]
           L3HostOpr $unit {ADD} $ip4Addr1 $nhIdx
           lappend host4L $ip4Addr1
           set nh($ip4Addr1) $nhIdx
           set ip4Addr1 [Ip4Opr $ip4Addr1 {+} 2]
           set nhMac  [MacOpr $nhMac {+} 2]
       }
       
       LogTest "Create L3_ENTRY_IPV6_UNICAST entries"
       set host6L ""
       for {set cntr 0} {$cntr < $count} {incr cntr} {
           LogTest "Create Egress Object"
           set nhIdx [L3EgressCreate $unit $nhMac $vid2 $port2 $intfId2 -1]
           L3HostOpr $unit {ADD} $ip6Addr1 $nhIdx
           lappend host6L $ip6Addr1
           set nh($ip6Addr1) $nhIdx
           set ip6Addr1 [Ip6Opr $ip6Addr1 {+} 2]
           set nhMac  [MacOpr $nhMac {+} 2]
       }
       
       LogTest "Form Ipv4 unicast packets"   
       set Ip4UcastPKTLIST ""
       foreach dip $host4L {    
          set data [PktDataBuild {L3V4} -src $smac -dest $intfMac1 -vid $vid1 -sip $sip4 -dip $dip]
          PktTxReady $unit PKT $data port $port1
          set pkt($dip) $PKT
          lappend Ip4UcastPKTLIST $PKT    
       }
       
       LogTest "Send Ipv4 unicast packets"  
       SnmpStatClear $unit $pL 
       PktTxList $unit $Ip4UcastPKTLIST 
       StatSnmpCheckClear $unit $port1 IfInUcastPkts $count $port2 IfOutUcastPkts $count
       
       LogTest "Form Ipv6 unicast packets"   
       set Ip6UcastPKTLIST ""
       foreach dip $host6L {    
         set data [PktDataBuild {L3V6} -src $smac -dest $intfMac1 -vid $vid1 -sip6 $sip6 -dip6 $dip]
         PktTxReady $unit PKT $data port $port1
         set pkt($dip) $PKT
         lappend Ip6UcastPKTLIST $PKT    
        }       
       
       SnmpStatClear $unit $pL 
       PktTxList $unit $Ip6UcastPKTLIST 
       StatSnmpCheckClear $unit $port1 IfInUcastPkts $count $port2 IfOutUcastPkts $count   
       
       CMD_E_NONE {bcm_ipmc_enable $unit $::TRUE}
       LogTest "Create $count Multicast Groups"
       for {set cntr 0} {$cntr < [expr 2*$count]} {incr cntr} {
            set ipGrp [McastGroupCreate $unit {L3}]
            append ipGrpL "$ipGrp "
        }
            
       LogTest "Create L3_ENTRY_IPV4_MULTICAST entries"
       set mcast4L ""; set mcip4L ""; set ip4L ""    
       set ip4GrpL [lrange $ipGrpL 0 [expr $count-1]]
       foreach ip4Grp $ip4GrpL {      
           L3IpmcOpr $unit {ADD} $mcip4Addr $ip4Grp $ip4Addr2 $vrf $vid1 $port1 
           lappend ip4L $ip4Addr2
           lappend mcip4L $mcip4Addr 
           lappend mcast4L $McastMac4      
           set Grp($mcip4Addr) $ip4Grp
           set Ip($mcip4Addr) $ip4Addr2
           set ip4Addr2 [Ip4Opr $ip4Addr2 {+} 2]
           set mcip4Addr [Ip4Opr $mcip4Addr {+} 2]
           set McastMac4 [MacOpr $McastMac4 {+} 2]     
       }
       
       LogTest "Create L3_ENTRY_IPV6_MULTICAST entries"
       set mcast6L ""; set mcip6L ""; set ip6L ""
       set ip6GrpL [lrange $ipGrpL $count end]  
       foreach ip6Grp $ip6GrpL {      
           L3IpmcOpr $unit {ADD} $mcip6Addr $ip6Grp $ip6Addr2 $vrf $vid1 $port1 
           lappend ip6L $ip6Addr2
           lappend mcip6L $mcip6Addr 
           lappend mcast6L $McastMac6      
           set Grp($mcip6Addr) $ip6Grp
           set Ip($mcip6Addr) $ip6Addr2
           set ip6Addr2 [Ip6Opr $ip6Addr2 {+} 2]
           set mcip6Addr [Ip6Opr $mcip6Addr {+} 2]
           set McastMac6 [MacOpr $McastMac6 {+} 2]     
       }
       
       LogTest "Add Ports to Multicast group"
       if {[DeviceSupport $unit GREYHOUND]} {
           set flags [expr $::BCM_L3_KEEP_DSTMAC|$::BCM_L3_IPMC]
           set egr_obj1 [L3EgressCreate $unit mac_addr $nhMac vlan $vid2 port $port2 intf $intfId2 flags $flags]
        } else {
           set egr_obj1 $intfId2
        }
       foreach ipGrp $ipGrpL {
         CMD_E_NONE {bcm_multicast_l3_encap_get $unit $ipGrp $gport2 $egr_obj1 encap1}
         CMD_E_NONE {bcm_multicast_egress_add $unit $ipGrp $gport2 $encap1}       
       }
       
       LogTest "Form Ipv4 mcast packets"   
       set IP4McastPKTLIST ""
       foreach dmac $mcast4L sip $ip4L dip $mcip4L {
         set data [PktDataBuild {L3V4} -src $smac -dest $dmac -vid $vid1 -sip $sip -dip $dip]
         PktTxReady $unit PKT $data port $port1
         set pkt($dip) $PKT      
         lappend IP4McastPKTLIST $PKT
       }
       
       LogTest "Send Ipv4 mcast packets"  
       SnmpStatClear $unit $pL
       PktTxList $unit $IP4McastPKTLIST
       StatSnmpCheckClear $unit $port1 IfInNUcastPkts $count $port2 IfOutNUcastPkts $count
       
       LogTest "Form Ipv6 mcast packets"   
       set IP6McastPKTLIST ""
       foreach dmac $mcast6L sip $ip6L dip $mcip6L {
          set data [PktDataBuild {L3V6} -src $smac -dest $dmac -vid $vid1 -sip6 $sip -dip6 $dip]
          PktTxReady $unit PKT $data port $port1
          set pkt($dip) $PKT      
          lappend IP6McastPKTLIST $PKT
       }
          
       LogTest "Send Ipv6 mcast packets"  
       SnmpStatClear $unit $pL
       PktTxList $unit $IP6McastPKTLIST
       StatSnmpCheckClear $unit $port1 IfInNUcastPkts $count $port2 IfOutNUcastPkts $count
      
       LogTest "Introduce Error at one entry of L3_ENTRY_IPV4_UNICAST"
       set errIp4 [RandSelect $host4L] 
       set Idx [SocTableIdxGet $unit $mem1 {IPV4UC} "$errIp4 $vrf"]
       LogTest "Introduce Error at index=$Idx"
       SerErrorInjectIndex $unit 0 $mem1Val $Idx $pipe_inject
       set regMemInfo "-matchData \{MEM $mem1Val $Idx\}"
       SerEventRegister $unit $regMemInfo
       LogTest "Send traffic to hit the entry"
       PktTx $unit $pkt($errIp4)
       SerProcessEvent $unit $::TRUE $regMemInfo
       L3HostCmp $unit ip_addr $errIp4 vrf $vrf intf $nh($errIp4)
       
       LogTest "Introduce Error at one entry of L3_ENTRY_IPV6_UNICAST"
       set errIp6 [RandSelect $host6L] 
       set Idx [SocTableIdxGet $unit $mem2 {IPV6UC} "$errIp6 $vrf"]
       LogTest "Introduce Error at index=$Idx"
       SerErrorInjectIndex $unit 0 $mem2Val $Idx $pipe_inject
       set regMemInfo "-matchData \{MEM $mem2Val $Idx\}"
       SerEventRegister $unit $regMemInfo
       LogTest "Send traffic to hit the entry"
       PktTx $unit $pkt($errIp6)
       SerProcessEvent $unit $::TRUE $regMemInfo
       L3HostCmp $unit ip6_addr $errIp6 vrf $vrf intf $nh($errIp6)
       
       LogTest "Introduce Error at one entry of L3_ENTRY_IPV4_MULTICAST"
       set errMcIp4 [RandSelect $mcip4L]
       set key "$errMcIp4 $Ip($errMcIp4) $vrf $vid1"
       set Idx [SocTableIdxGet $unit $mem3 {IPV4MC} $key]
       LogTest "Introduce Error at index=($Idx)"
       SerErrorInjectIndex $unit 0 $mem3Val $Idx $pipe_inject
       set regMemInfo "-matchData {MEM $mem3Val $Idx}"
       SerEventRegister $unit $regMemInfo
       PktTx $unit $pkt($errMcIp4)
       SerProcessEvent $unit $::TRUE $regMemInfo
       L3IpmcOpr $unit {Find} $errMcIp4 $Grp($errMcIp4) $Ip($errMcIp4) $vrf $vid1 $port1

       LogTest "Introduce Error at one entry of L3_ENTRY_IPV6_MULTICAST"
       set errMcIp6 [RandSelect $mcip6L]
       set key "$errMcIp6 $Ip($errMcIp6) $vrf $vid1"
       set Idx [SocTableIdxGet $unit $mem4 {IPV6MC} $key]
       LogTest "Introduce Error at index=($Idx)"
       SerErrorInjectIndex $unit 0 $mem4Val $Idx $pipe_inject
       set regMemInfo "-matchData {MEM $mem4Val $Idx}"
       SerEventRegister $unit $regMemInfo
       PktTx $unit $pkt($errMcIp6)
       SerProcessEvent $unit $::TRUE $regMemInfo
       L3IpmcOpr $unit {Find} $errMcIp6 $Grp($errMcIp6) $Ip($errMcIp6) $vrf $vid1 $port1       

       if {!$check} {
          PktMemFree $unit "$IP6McastPKTLIST $IP4McastPKTLIST \
                            $Ip4UcastPKTLIST $Ip6UcastPKTLIST"
          unset IP6McastPKTLIST
          unset IP4McastPKTLIST
          unset Ip4UcastPKTLIST
          unset Ip6UcastPKTLIST
          
          LogPuts "Introduce Error for one entry of $mem1 before warmboot"
          set errIp4 [RandSelect $host4L] 
          set Idx [SocTableIdxGet $unit $mem1 {IPV4UC} "$errIp4 $vrf"]
          SerErrorInjectIndex $unit 0 $mem1Val $Idx $pipe_inject
          
          LogPuts "Introduce Error for one entry of $mem2 before warmboot"
          set errIp6 [RandSelect $host6L]   
          set Idx [SocTableIdxGet $unit $mem2 {IPV6UC} "$errIp6 $vrf"]
          SerErrorInjectIndex $unit 0 $mem2Val $Idx $pipe_inject 
          
          LogPuts "Introduce Error for one entry of $mem3 before warmboot"
          set errMcIp4 [RandSelect $mcip4L]
          set key "$errMcIp4 $Ip($errMcIp4) $vrf $vid1"
          set Idx [SocTableIdxGet $unit $mem3 {IPV4MC} $key]
          SerErrorInjectIndex $unit 0 $mem3Val $Idx $pipe_inject          

          LogPuts "Introduce Error for each entry of $mem4 before warmboot"
          set errMcIp6 [RandSelect $mcip6L]
          set key "$errMcIp6 $Ip($errMcIp6) $vrf $vid1"
          set Idx [SocTableIdxGet $unit $mem4 {IPV6MC} $key]
          SerErrorInjectIndex $unit 0 $mem4Val $Idx $pipe_inject              
       }
       SaveTestVars
  }
  LoadTestVars
  if {$check} {
       SocControlSet $unit {FORCE_READ_THROUGH}  $::FALSE
       foreach Ip4 $host4L {
          L3HostCmp $unit ip_addr $Ip4 vrf $vrf $nh($Ip4)
       }
       foreach Ip6 $host6L {
          L3HostCmp $unit ip6_addr $Ip6 vrf $vrf $nh($Ip6)
       }
       foreach mcip "$mcip4L $mcip6L" {
          L3IpmcOpr $unit {Find} $mcip $Grp($mcip) $Ip($mcip) $vrf $vid1 $port1
       } 
       
       LogTest "Form Ipv4 unicast packets"   
       set Ip4UcastPKTLIST ""
       foreach dip $host4L {    
          set data [PktDataBuild {L3V4} -src $smac -dest $intfMac1 -vid $vid1 -sip $sip4 -dip $dip]
          PktTxReady $unit PKT $data port $port1
          set pkt($dip) $PKT
          lappend Ip4UcastPKTLIST $PKT    
       }
       
       LogTest "Send Ipv4 unicast packets"  
       SnmpStatClear $unit $pL 
       PktTxList $unit $Ip4UcastPKTLIST 
       StatSnmpCheckClear $unit $port1 IfInUcastPkts $count $port2 IfOutUcastPkts $count
       
       LogTest "Introduce Error at one entry of L3_ENTRY_IPV4_UNICAST"
       set errIp4 [RandSelect $host4L] 
       set Idx [SocTableIdxGet $unit $mem1 {IPV4UC} "$errIp4 $vrf"]
       LogTest "Introduce Error at index=$Idx"
       SerErrorInjectIndex $unit 0 $mem1Val $Idx $pipe_inject
       set regMemInfo "-matchData \{MEM $mem1Val $Idx\}"
       SerEventRegister $unit $regMemInfo
       LogTest "Send traffic to hit the entry"
       PktTx $unit $pkt($errIp4)
       SerProcessEvent $unit $::TRUE $regMemInfo
       L3HostCmp $unit ip_addr $errIp4 vrf $vrf $nh($errIp4)
              
       SnmpStatClear $unit $pL 
       PktTxList $unit $Ip4UcastPKTLIST 
       StatSnmpCheckClear $unit $port1 IfInUcastPkts $count $port2 IfOutUcastPkts $count
       
       LogTest "Form Ipv6 unicast packets"   
       set Ip6UcastPKTLIST ""
       foreach dip $host6L {    
         set data [PktDataBuild {L3V6} -src $smac -dest $intfMac1 -vid $vid1 -sip6 $sip6 -dip6 $dip]
         PktTxReady $unit PKT $data port $port1
         set pkt($dip) $PKT
         lappend Ip6UcastPKTLIST $PKT    
       }       
       
       SnmpStatClear $unit $pL 
       PktTxList $unit $Ip6UcastPKTLIST 
       StatSnmpCheckClear $unit $port1 IfInUcastPkts $count $port2 IfOutUcastPkts $count 
       
       LogTest "Introduce Error at one entry of L3_ENTRY_IPV6_UNICAST"
       set errIp6 [RandSelect $host6L] 
       set Idx [SocTableIdxGet $unit $mem2 {IPV6UC} "$errIp6 $vrf"]
       LogTest "Introduce Error at index=$Idx"
       SerErrorInjectIndex $unit 0 $mem2Val $Idx $pipe_inject
       set regMemInfo "-matchData \{MEM $mem2Val $Idx\}"
       SerEventRegister $unit $regMemInfo
       LogTest "Send traffic to hit the entry"
       PktTx $unit $pkt($errIp6)
       SerProcessEvent $unit $::TRUE $regMemInfo
       L3HostCmp $unit ip6_addr $errIp6 vrf $vrf $nh($errIp6)
       
       SnmpStatClear $unit $pL 
       PktTxList $unit $Ip6UcastPKTLIST 
       StatSnmpCheckClear $unit $port1 IfInUcastPkts $count $port2 IfOutUcastPkts $count 
       
       LogTest "Form Ipv4 mcast packets"   
       set IP4McastPKTLIST ""
       foreach dmac $mcast4L sip $ip4L dip $mcip4L {
         set data [PktDataBuild {L3V4} -src $smac -dest $dmac -vid $vid1 -sip $sip -dip $dip]
         PktTxReady $unit PKT $data port $port1
         set pkt($dip) $PKT      
         lappend IP4McastPKTLIST $PKT
       }
       
       LogTest "Send Ipv4 mcast packets"  
       SnmpStatClear $unit $pL
       PktTxList $unit $IP4McastPKTLIST
       StatSnmpCheckClear $unit $port1 IfInNUcastPkts $count $port2 IfOutNUcastPkts $count
       
       LogTest "Introduce Error at one entry of L3_ENTRY_IPV4_MULTICAST"
       set errMcIp4 [RandSelect $mcip4L]
       set key "$errMcIp4 $Ip($errMcIp4) $vrf $vid1"
       set Idx [SocTableIdxGet $unit $mem3 {IPV4MC} $key]
       LogTest "Introduce Error at index=($Idx)"
       SerErrorInjectIndex $unit 0 $mem3Val $Idx $pipe_inject
       set regMemInfo "-matchData {MEM $mem3Val $Idx}"
       SerEventRegister $unit $regMemInfo
       PktTx $unit $pkt($errMcIp4)
       SerProcessEvent $unit $::TRUE $regMemInfo
       L3IpmcOpr $unit {Find} $errMcIp4 $Grp($errMcIp4) $Ip($errMcIp4) $vrf $vid1 $port1
              
       SnmpStatClear $unit $pL
       PktTxList $unit $IP4McastPKTLIST
       StatSnmpCheckClear $unit $port1 IfInNUcastPkts $count $port2 IfOutNUcastPkts $count
       
       LogTest "Form Ipv6 mcast packets"   
       set IP6McastPKTLIST ""
       foreach dmac $mcast6L sip $ip6L dip $mcip6L {
          set data [PktDataBuild {L3V6} -src $smac -dest $dmac -vid $vid1 -sip6 $sip -dip6 $dip]
          PktTxReady $unit PKT $data port $port1
          set pkt($dip) $PKT      
          lappend IP6McastPKTLIST $PKT
       }
          
       LogTest "Send Ipv6 mcast packets"  
       SnmpStatClear $unit $pL
       PktTxList $unit $IP6McastPKTLIST
       StatSnmpCheckClear $unit $port1 IfInNUcastPkts $count $port2 IfOutNUcastPkts $count
       
       LogTest "Introduce Error at one entry of L3_ENTRY_IPV6_MULTICAST"
       set errMcIp6 [RandSelect $mcip6L]
       set key "$errMcIp6 $Ip($errMcIp6) $vrf $vid1"
       set Idx [SocTableIdxGet $unit $mem4 {IPV6MC} $key]
       LogTest "Introduce Error at index=($Idx)"
       SerErrorInjectIndex $unit 0 $mem4Val $Idx $pipe_inject
       set regMemInfo "-matchData {MEM $mem4Val $Idx}"
       SerEventRegister $unit $regMemInfo 
       PktTx $unit $pkt($errMcIp6)
       SerProcessEvent $unit $::TRUE $regMemInfo
       L3IpmcOpr $unit {Find} $errMcIp6 $Grp($errMcIp6) $Ip($errMcIp6) $vrf $vid1 $port1
       
       SnmpStatClear $unit $pL
       PktTxList $unit $IP6McastPKTLIST
       StatSnmpCheckClear $unit $port1 IfInNUcastPkts $count $port2 IfOutNUcastPkts $count
       
  }
  if {$cleanup} {
       LogTest "Cleanup"
       PktMemFree $unit "$IP6McastPKTLIST $IP4McastPKTLIST \
                         $Ip4UcastPKTLIST $Ip6UcastPKTLIST"
       L3ConfigReset $unit
       L2ConfigReset $unit
       PortConfigReset $unit $pL
    }   
}
    

#warmboot setup test
proc WarmBootNS::AT_ser_wb_L_001_Setup {chip unit} {
    #NTestSeedInit need within the test and not in a library
    NTestSeedInit    
    AT_SER_Tr_L3Entry $chip $unit 1 0 0
    return PASS
}

#warmboot check test
proc WarmBootNS::AT_ser_wb_L_001_Check {chip unit} {
    AT_SER_Tr_L3Entry $chip $unit 0 1 1
    return PASS
}

#
# @ Traffic Tested:
#
# @Purpose:
#    Test to verify WARMBOOT SER handling for L2x
# @Steps:
#    1. Create Multiple entries for L2x
#    2. Introduce error for each entry
#    3. Send packet to hit the errored entry
#    4. Verify packet fowarding is OK after errored entry restored.
#    5. Introduce the error again, then warmboot
#    6. After warmboot, use api to check
#    7. Introduce errors in the entries
#    8. Send packet to hit the errored entries
#    9. Verify packet fowarding is OK after errored entries restored.
#    10. Clean the configuration
proc AT_SER_Tr_L2x {chip unit setup check cleanup} {
    # SER Support Check
    FEATURE_SUPPORT $unit {SER}

    # Set Logging Level
    LogLevelSet Silent
    
    
    if {$setup} { 
      LogTest "Stop L2X thread"
      soc_l2x_stop $unit
      
      #var
      set mac           [MacRand]
      set smac          00:00:00:00:00:11
      set flgDy         0
      set flgSt         $::BCM_L2_STATIC
      set vid           [VlanRandIdGet $unit 1]
      set count         5                  ;# count of entries created
      set mem           L2X
      set memVal        $::L2Xm
      set p_cnt         3
      set pipe_inject   -1
      # Enable soc property force_read_through
      SocControlSet $unit {FORCE_READ_THROUGH}  $::TRUE 
      
      #Log Memory being Tested 
      LogPuts "\[SER\]: Memory Tested=($mem), ID=($memVal)" 
      
      # Switch event map
      SwitchEventMap $unit
      
      LogTest "Select $p_cnt Ports"
      set pL [PortSelect $unit $p_cnt {-e} -nomodidset -tx_ready -no_remove_ports -no_remove_cpu] 
      LogTest "Selected ports=($pL)"
      ListVarsAssign $pL {port1 port2 port3}
      set gL [PortGportGet $unit $pL]
      ListVarsAssign $gL {gport1 gport2 gport3}  
      
      #select pipe to inject error if mutipipe is supported
      if {[ChipModVarGet $unit {DEV} multi_pipe_support]} {
          set pipe_inject [PortListPipeGet $unit $gport1]
      }
      
      LogTest "Create Vlan"
      VlanCreatePortAdd $unit $vid "$pL"
      
      #Add smac to l2_entry to avoid traffic storm 
      L2AddrCreate $unit $smac $vid $port1 $flgSt
      
      LogTest "Create $count Dynamic Entries"
      for {set cntr 0} {$cntr < $count} {incr cntr} {
          L2AddrCreate $unit $mac $vid $port2 $flgDy
          append macDyList "$mac "
          set mac [MacOpr $mac {+}]
      }
      
      LogTest "Create $count STATIC Entries"
      for {set cntr 0} {$cntr < $count} {incr cntr} {
          L2AddrCreate $unit $mac $vid $port2 $flgSt
          append macStList "$mac "
          set mac [MacOpr $mac {+}]
      }
      
      LogTest "Create Hw Map"
      foreach mac "$macStList $macDyList" {
          set key "$mac $vid"
          set hwIdx($mac) [SocTableIdxGet $unit $mem {L2} $key]
      }
      
      set PKTLIST ""
      foreach mac "$macStList $macDyList" { 
          set data [PktDataBuild {L2ETHII} -src $smac -dest $mac -vid $vid]
          PktTxReady $unit PKT $data port $port1
          set pkt($mac) $PKT
          lappend PKTLIST $PKT
       }
      
      LogTest "Send packets"      
      SnmpStatClear $unit $pL 
      PktTxList $unit $PKTLIST 
      StatSnmpCheckClear $unit $port1 IfInUcastPkts [expr 2*$count]  \
                               $port2 IfOutUcastPkts [expr 2*$count] \
                               $port3 IfOutUcastPkts 0
      
      set mac [RandSelect $macStList]
      set errIdx $hwIdx($mac)
      LogTest "Introduce Error at index=$errIdx"
      SerErrorInjectIndex $unit 0 $memVal $errIdx $pipe_inject
      
      set regMemInfo "-matchData \{MEM $memVal $errIdx\}"
      SerEventHandler $unit $::TRUE $regMemInfo
      
      LogTest "Send traffic to hit the entry"
      PktTx $unit $pkt($mac)
      SerProcessEvent $unit $::TRUE $regMemInfo
      L2AddrVerify $unit $mac $vid
      
      SnmpStatClear $unit $pL
      PktTxList $unit $PKTLIST 
      StatSnmpCheckClear $unit $port1 IfInUcastPkts [expr 2*$count]  \
                               $port2 IfOutUcastPkts [expr 2*$count] \
                               $port3 IfOutUcastPkts 0
          
      set mac1  [RandSelect $macStList]
      set mac2  [RandSelectConsume macDyList]
      set errIdx1 $hwIdx($mac1)
      set errIdx2 $hwIdx($mac2)
      LogTest "Introduce Error at index=$errIdx1,$errIdx2"
      SerErrorInjectIndex $unit 0 $memVal $errIdx1 $pipe_inject
      SerErrorInjectIndex $unit 0 $memVal $errIdx2 $pipe_inject
      PktTx $unit $PKTLIST
      StatSnmpCheckClear $unit $port2 IfOutUcastPkts [expr 2*$count - 2]
      L2AddrVerify $unit $mac1 $vid
      L2AddrVerify $unit $mac2 $vid 0
      
      SnmpStatClear $unit $pL
      PktTxList $unit $PKTLIST
      StatSnmpCheckClear $unit $port1 IfInUcastPkts [expr 2*$count]  \
                               $port2 IfOutUcastPkts [expr 2*$count] \
                               $port3 IfOutUcastPkts 1
      
      if {!$check} { 
          PktMemFree $unit $PKTLIST
          unset PKTLIST
          LogPuts "Introduce Error for one of static entries before warmboot"
          set mac [RandSelect $macStList]
          set errIdx [SocTableIdxGet $unit $mem {L2} "$mac $vid"]
          SerErrorInjectIndex $unit 0 $memVal $errIdx $pipe_inject
          
          LogPuts "Introduce Error for one of dynamic entries before warmboot"
          set mac [RandSelectConsume macDyList]
          set errIdx [SocTableIdxGet $unit $mem {L2} "$mac $vid"]
          SerErrorInjectIndex $unit 0 $memVal $errIdx $pipe_inject            
      }
      SaveTestVars    
    }
    LoadTestVars
    if {$check} {
         SocControlSet $unit {FORCE_READ_THROUGH}  $::FALSE
         foreach mac "$macStList $macDyList" {
           L2AddrVerify $unit $mac $vid
         }
         
         set PKTLIST ""
         foreach mac "$macStList $macDyList" { 
            set data [PktDataBuild {L2ETHII} -src $smac -dest $mac -vid $vid]
            PktTxReady $unit PKT $data port $port1
            set pkt($mac) $PKT
            lappend PKTLIST $PKT
         }
         SnmpStatClear $unit $pL
         PktTxList $unit $PKTLIST
         StatSnmpCheckClear $unit $port1 IfInUcastPkts [expr 2*$count -2]  \
                                  $port2 IfOutUcastPkts [expr 2*$count -2] \
                                  $port3 IfOutUcastPkts 0
                                  
         set mac1  [RandSelect $macStList]
         set mac2  [RandSelectConsume macDyList]
         set errIdx1 $hwIdx($mac1)
         set errIdx2 $hwIdx($mac2)
         LogTest "Introduce Error at index=$errIdx1,$errIdx2"
         SerErrorInjectIndex $unit 0 $memVal $errIdx1 $pipe_inject
         SerErrorInjectIndex $unit 0 $memVal $errIdx2 $pipe_inject
         PktTx $unit $PKTLIST
         StatSnmpCheckClear $unit $port2 IfOutUcastPkts [expr 2*$count - 4]
         L2AddrVerify $unit $mac1 $vid
         L2AddrVerify $unit $mac2 $vid 0
         
         SnmpStatClear $unit $pL
         PktTxList $unit $PKTLIST
         StatSnmpCheckClear $unit $port1 IfInUcastPkts [expr 2*$count -2]  \
                                  $port2 IfOutUcastPkts [expr 2*$count -2] \
                                  $port3 IfOutUcastPkts 1               
    }
    
     if {$cleanup} {
           LogTest "Cleanup"
           PktMemFree $unit $PKTLIST 
           L2ConfigReset $unit
           PortConfigReset $unit $pL
           return PASS
     }
}

#warmboot setup test
proc WarmBootNS::AT_ser_wb_L_002_Setup {chip unit} {
    #NTestSeedInit need within the test and not in a library
    NTestSeedInit
    AT_SER_Tr_L2x $chip $unit 1 0 0
    return PASS
}

#warmboot check test
proc WarmBootNS::AT_ser_wb_L_002_Check {chip unit} {
    AT_SER_Tr_L2x $chip $unit 0 1 1
    return PASS
} 

proc AT_SER_All_Mem {chip unit setup check cleanup} {

    FEATURE_SUPPORT $unit {SER}

    #Currently only support TD2, will support other chips in future
    if {![DeviceSupport $unit {TRIDENT2 TOMAHAWK TRIDENT2PLUS TRIDENT}]} {
         NTest::Error NO_SUPPORT "Currently only support Trident family" $::NO_RESET
    }
    cmd memscan off
    soc_l2x_stop $unit
    TridentFamilyIPEPMemoryGet $unit memList
    #No need to split mem tables, as it could handle in scrum of 8 depth FIFO
    #TridentFamilyIPEPMemoryGetIndexPart $unit memList $index $items
    # Set Logging Level
    LogLevelSet Silent

    SocControlSet $unit {FORCE_READ_THROUGH}  $::TRUE

    set errIdx 1
    set errorMems ""
    set rv 0
    set pattern "%-50s %-2s"
    # Memory being Tested
    foreach {mem memVal} $memList {
        set dumpNew 0
        set errIdxNew 1
        # Need first unregister the ser event, otherwise if the memory already register and run failed,
        #then you want run it again, will failed.
        if {$setup} {
            SerEventUnRegister $unit
        }

        LogPuts "-----$mem test begin------"
        LogPuts "\[SER\]: Memory Tested=($mem), ID=($memVal)"
        set memValOrignal $memVal
        switch $mem {
            "L3_ENTRY_IPV4_MULTICAST" - "L3_ENTRY_IPV6_UNICAST" {
                if {[DeviceSupport $unit {TOMAHAWK TRIDENT2PLUS}]} {
                    set errIdxNew 2
                    set memVal $::L3_ENTRY_ONLY_ECCm
                } elseif {[DeviceSupport $unit {TRIDENT2}]} {
                    set errIdxNew 2
                    set memVal $::L3_ENTRY_ONLYm
                }
            }
            "L3_ENTRY_IPV6_MULTICAST" {
                if {[DeviceSupport $unit {TOMAHAWK TRIDENT2PLUS}]} {
                    set errIdxNew 4
                    set memVal $::L3_ENTRY_ONLY_ECCm
                } elseif {[DeviceSupport $unit {TRIDENT2}]} {
                    set errIdxNew 4
                    set memVal $::L3_ENTRY_ONLYm
                }
            }
            "L3_ENTRY_IPV4_UNICAST" {
                if {[DeviceSupport $unit {TOMAHAWK TRIDENT2PLUS}]} {
                    set memVal $::L3_ENTRY_ONLY_ECCm
                } elseif {[DeviceSupport $unit {TRIDENT2}]} {
                    set memVal $::L3_ENTRY_ONLYm
                }
            }
            "L3_ENTRY_ONLY" {
                if {[DeviceSupport $unit {TOMAHAWK TRIDENT2PLUS}]} {
                    set memVal $::L3_ENTRY_ONLY_ECCm
                } elseif {[DeviceSupport $unit {TRIDENT}]} {
                    set memVal $::L3_ENTRY_IPV4_UNICASTm
                }
            }
            "L2_USER_ENTRY_DATA_ONLY" {
                if {[DeviceSupport $unit {TRIDENT2 TRIDENT2PLUS TRIDENT TOMAHAWK}]} {
                    set memVal $::L2_USER_ENTRYm
                    continue
                }
            }
            "VLAN_SUBNET_DATA_ONLY" {
                if {[DeviceSupport $unit {TRIDENT2 TRIDENT2PLUS TRIDENT TOMAHAWK}]} {
                    set memVal $::VLAN_SUBNETm
                }
            }
            "ING_SNAT_DATA_ONLY" {
                if {[DeviceSupport $unit {TRIDENT2 TRIDENT2PLUS TRIDENT TOMAHAWK}]} {
                    set memVal $::ING_SNATm
                }
            }
            "MY_STATION_TCAM_DATA_ONLY" {
                if {[DeviceSupport $unit {TRIDENT2 TRIDENT2PLUS TRIDENT TOMAHAWK}]} {
                    set memVal $::MY_STATION_TCAMm
                }
            }
            "MODPORT_MAP_M0" - "MODPORT_MAP_M1" - "MODPORT_MAP_M2" - "MODPORT_MAP_M3" {
                if {[DeviceSupport $unit {TRIDENT2 TRIDENT2PLUS TRIDENT TOMAHAWK}]} {
                    set memVal $::MODPORT_MAP_MIRRORm
                }
            }
            "EGR_IP_TUNNEL_IPV6" {
                set dumpNew 1
                if {[DeviceSupport $unit {TRIDENT}]} {
                    set memVal $::EGR_IP_TUNNELm
                    set errIdxNew 0
                    #known issue
                    #SerErrorInjectIndex $unit 1 $memVal 0
                    #no ser event
                    #continue
                } elseif {[DeviceSupport $unit {TOMAHAWK TRIDENT2PLUS}]} {
                    set memVal $::EGR_IP_TUNNELm
                    set errIdxNew 2
                } elseif {[DeviceSupport $unit {TRIDENT2}]} {
                    set memVal $::EGR_IP_TUNNEL_IPV6m
                }
            }
            "EGR_IP_TUNNEL_MPLS" {
                set dumpNew 1
                if {[DeviceSupport $unit {TRIDENT}]} {
                    set memVal $::EGR_IP_TUNNELm
                    set errIdxNew 0
                    #Known issue
                    #SerErrorInjectIndex $unit 1 $memVal 0
                    #no ser event
                    #continue
                } elseif {[DeviceSupport $unit {TOMAHAWK TRIDENT2PLUS}]} {
                    set memVal $::EGR_IP_TUNNELm
                } elseif {[DeviceSupport $unit {TRIDENT2}]} {
                    set memVal $::EGR_IP_TUNNEL_IPV6m
                    set errIdxNew 0
                }
            }
            "EGR_IP_TUNNEL" {
                set dumpNew 1
                if {[DeviceSupport $unit {TRIDENT2}]} {
                    set memVal $::EGR_IP_TUNNEL_IPV6m
                    set errIdxNew 0
                } elseif {[DeviceSupport $unit {TRIDENT}]} {
                    #no ser event
                    #continue
                    set errIdxNew 0
                    #known issue
                    #SerErrorInjectIndex $unit 1 $memVal 0
                }
            }
            "EFP_METER_TABLE" {
                if {[DeviceSupport $unit {TRIDENT2PLUS TRIDENT2}]} {
                    set memVal $::EFP_METER_TABLE_Xm
                }
            }
            "FP_STORM_CONTROL_METERS" {
                if {[DeviceSupport $unit {TRIDENT2PLUS TRIDENT2}]} {
                    set memVal $::FP_STORM_CONTROL_METERS_Xm
                }
            }
            "EGR_FRAGMENT_ID_TABLE" {
                if {[DeviceSupport $unit {TRIDENT2PLUS TRIDENT2}]} {
                    set memVal $::EGR_FRAGMENT_ID_TABLE_Xm
                }
            }
            "MPLS_ENTRY" {
                if {[DeviceSupport $unit {TOMAHAWK}]} {
                    set memVal $::MPLS_ENTRY_ECCm
                }
            }
            "L2_ENTRY_TILE" {
                if {[DeviceSupport $unit {TOMAHAWK}]} {
                    set memVal $::L2Xm
                }
            }
            "L3_TUNNEL_DATA_ONLY" {
                if {[DeviceSupport $unit {TOMAHAWK}]} {
                    set memVal $::L3_TUNNELm
                }
            }
            "DLB_HGT_FLOWSET" {
                if {[DeviceSupport $unit {TRIDENT2PLUS}]} {
                    set memVal $::DLB_HGT_FLOWSET_Xm
                }
            }
            default {
            }
        }
        LogPuts "regmemInfo: errIdxNew=$errIdxNew memVal=$memVal"
        set regMemInfo "-matchData {MEM $memVal $errIdxNew}"
        # register event handler, cannot use SerEventHandler as it call TeardownFuncRegister
        #Which will have issue
        #SerEventHandler $unit $::TRUE $regMemInfo
        #SerEventParamsReset
        SerEventRegister $unit $regMemInfo
        set ::gSerContinue 1
        LogPuts "\[SER\]:In AT_SER_Mem_All gSerContinue = $::gSerContinue"
        if {$setup} {    
            #I move it to here after register, as some meter tables will update by HW
            set injectRes [SerErrorInjectIndex $unit 0 $memValOrignal $errIdx]
            if {$injectRes != $::CMD_SUCCESS} {
                set rv 1
                set result "soc_ser_inject_error return failed: $injectRes"                          
                #continue
            } else {
                set result "SER inject PASS"
            }
            append errorMems "   [format $pattern  $mem  $result]\n"
            LogPuts "-----$mem test end------"
            after 200
        }
        if {$check} {
            if {[expr $dumpNew == 1]} {
                cmd d nocache chg $mem $errIdxNew 1
                cmd d nocache chg $mem $errIdx 1
            } else {
                cmd d nocache chg $mem $errIdx 1
            }
            
            after 100
            set result [SerProcessEvent $unit $::TRUE $regMemInfo]
            if {$result == $::CMD_SUCCESS} {
                set rv 1
                set result "Fail: SER event still reported after warm boot"
            } else {
                set result "PASS"
            }
            append errorMems "   [format $pattern  $mem  $result]\n"
            LogPuts "-----$mem test end------"
            after 500
        }
    }

    SerEventUnRegister $unit
    set ::gSerContinue 0

    LogPuts "-----------------------Tables test results as follows----------------------------"
    LogPuts $errorMems
    LogPuts "---------------------------------------------------------------------------------"
    if {$rv} {
        NTest::Error FAIL "Some tables SER warm boot failed"
    } else {
        return PASS
    }
}

#warmboot setup test
proc WarmBootNS::AT_ser_wb_Mem_All_Setup {chip unit } {
    #NTestSeedInit need within the test and not in a library
    NTestSeedInit
    AT_SER_All_Mem $chip $unit 1 0 0 
    return PASS
}

#warmboot check test
proc WarmBootNS::AT_ser_wb_Mem_All_Check {chip unit} {
    AT_SER_All_Mem $chip $unit 0 1 1 
    return PASS
}


##
# \brief SER Configuration Testcase

# @ Traffic Tested:
#
# @Purpose:
#    Test to verify WARMBOOT SER handling of memory=VLAN_TABLE/EGR_VLAN
#
# @Steps:
#    1. Create Multiple entries
#    2. Introduce error in one of the entries
#    3. Send packet to hit the errored entry
#    4. Verify packet fowarding is OK after errored entry restored.
#    5. Introduce the error again, then warmboot
#    6. After warmboot, use api to check
#    7. Introduce errors in two of the entries
#    8. Send packet to hit the errored entries
#    9. Verify packet fowarding is OK after errored entries restored.
#    10. Clean the configuration

proc AT_SER_Tr_VlanTab {chip unit setup check cleanup} {
    # SER Support Check
    FEATURE_SUPPORT $unit {SER}
    # Switch event map
    SwitchEventMap $unit

    # Set Logging Level
    LogLevelSet Silent
    # Enable soc property force_read_through
    SocControlSet $unit {FORCE_READ_THROUGH}  $::TRUE
    if {$setup} {
        #var
        set dmac          00:00:00:00:00:12
        set smac          00:00:00:00:01:11
        set count         10                  ;# count of entries created
        set p_cnt         2
        set pipe_inject   -1

        LogTest "Select $p_cnt Ports"
        set pL  [PortSelect $unit $p_cnt {-e} -nomodidset -tx_ready -no_remove_ports -no_remove_cpu]
        LogTest "Selected port=($pL)"
        ListVarsAssign $pL {port1 port2}
        set gL [PortGportGet $unit $pL]
        ListVarsAssign $gL {gport1 gport2}

        #select pipe to inject error if mutipipe is supported
        if {[ChipModVarGet $unit {DEV} multi_pipe_support]} {
            set pipeInjectPort1 [PortListPipeGet $unit $gport1]
            set pipeInjectPort2 [PortListPipeGet $unit $gport2]
        }
        set memList "VLAN_TAB $::VLAN_TABm $pipeInjectPort1 \
                     EGR_VLAN $::EGR_VLANm $pipeInjectPort2"
        LogTest "Create Vlan entries"
        set vidL [VlanRandIdGet $unit $count]
        set vidLSave $vidL
        foreach vid $vidL {
            L2AddrCreate $unit $smac $vid $port1 $::BCM_L2_STATIC
            VlanCreatePortAdd $unit "$vid" "$pL"
        }

        set PKTLIST ""
        foreach vid $vidL {
            set data [PktDataBuild {L2ETHII} -src $smac -dest $dmac -vid $vid]
            PktTxReady $unit PKT $data port $port1
            set pkt($vid) $PKT
            lappend PKTLIST $PKT
        }

        LogTest "Send packets"
        SnmpStatClear $unit $pL
        PktTxList $unit $PKTLIST
        StatSnmpCheckClear $unit $port1 IfInUcastPkts $count $port2 IfOutUcastPkts $count

        set vid [RandSelectConsume vidL]
        set errIdx  $vid

        foreach {mem memVal pipe_inject} $memList {
            #Log Memory being Tested
            LogPuts "\[SER\]: Memory Tested=($mem), ID=($memVal)"

            LogTest "Introduce Error at index=($errIdx)"
            SerErrorInjectIndex $unit 0 $memVal $errIdx  $pipe_inject

            set regMemInfo "-matchData {MEM $memVal $errIdx}"
            if {[DeviceSupport $unit SABER2]} {
                #SDK-70526:SW have bug but won't fix
                #Cannot use inlineData or matchXorData as they match the same memory
                #Just the index different(which is used in hash table)
                #The listData count is 2 is that report EGR_VLAN and VLAN_TAB on SB2
                set regMemInfo "-listData {MEM $memVal 2}"
            }
            SerEventRegister $unit $regMemInfo

            PktTx $unit $pkt($vid)
            SerProcessEvent $unit $::TRUE $regMemInfo
            SnmpStatClear $unit $pL
            PktTxList $unit $PKTLIST
            StatSnmpCheckClear $unit $port1 IfInUcastPkts $count $port2 IfOutUcastPkts $count
        }
        if {!$check} {
            #In setup
            #packet structure cannot save, so free and unreset
            PktMemFree $unit $PKTLIST
            unset PKTLIST
            unset data
            unset PKT
            unset pkt
            set vidL $vidLSave
            unset vidLSave
            # Get the vlan_profile of the index to save
            LogTest "Get vlan_profile before inject error for vlan table"
            CMD_E_NONE {bcm_vlan_control_vlan_get $unit $vid vlanProfile}
            ChangeStructToList $vlanProfile vlanGetItemList vlanGetValueList
            foreach {mem memVal pipe_inject} $memList {
                #inject an error to that entry
                #it should be recover before system scrumb(before WB)
                LogPuts "Introduce Error at index=($errIdx)again on Table $mem before warmboot"
                SerErrorInjectIndex $unit 0 $memVal $errIdx $pipe_inject
            }

        }
        SaveTestVars
    }
    LoadTestVars
    if {$check} {
        if {!$setup} {
            #In check case:
            #It should be already recovered the SER error
            LogTest "Get the Vlan result after warmboot"
            CMD_E_NONE {bcm_vlan_control_vlan_get $unit $vid vlanProfileWB}
            ChangeStructToList $vlanProfileWB vlanGetItemWBList vlanGetValueWBList
            LogPuts "Before WB vlanGetValueList is  $vlanGetValueList"
            LogPuts "After WB vlanGetValueWBList is $vlanGetValueWBList"
            LogTest "Compare the results"
            foreach vlanGet $vlanGetValueList vlanGetWB $vlanGetValueWBList vlanGetItem $vlanGetItemList {
                NTest::AssertEq $vlanGet $vlanGetWB  FAIL "vlan $vid control Mismatch after warmboot \
                in $vlanGetItem of bcm_vlan_control_vlan_t, before WB is $vlanGet, after WB is $vlanGetWB"
            }
            foreach vid $vidL {
                set data [PktDataBuild {L2ETHII} -src $smac -dest $dmac -vid $vid]
                PktTxReady $unit PKT $data port $port1
                set pkt($vid) $PKT
                lappend PKTLIST $PKT
            }
        }
        set vid1 [RandSelectConsume vidL]
        set vid2 [RandSelectConsume vidL]
        set errIdx1  $vid1
        set errIdx2  $vid2
        foreach {mem memVal pipe_inject} $memList {
            SerErrorInjectIndex $unit 0 $memVal $errIdx1 $pipe_inject
            SerErrorInjectIndex $unit 0 $memVal $errIdx2 $pipe_inject
            PktTx $unit $PKTLIST
            StatSnmpCheckClear $unit $port2 IfOutUcastPkts [expr $count - 2]

            after 3000
            SnmpStatClear $unit $pL
            PktTxList $unit $PKTLIST
            StatSnmpCheckClear $unit $port1 IfInUcastPkts $count $port2 IfOutUcastPkts $count
        }
    }
    if {$cleanup} {
        LogTest "Clean up"
        PktMemFree $unit $PKTLIST
        L2ConfigReset $unit
        PortConfigReset $unit $pL
    }
    return PASS
}
#warmboot setup test
proc WarmBootNS::AT_ser_wb_V_001_Setup {chip unit} {
    #NTestSeedInit need within the test and not in a library
    NTestSeedInit
    AT_SER_Tr_VlanTab $chip $unit 1 0 0
    return PASS
}

#warmboot check test
proc WarmBootNS::AT_ser_wb_V_001_Check {chip unit} {
    AT_SER_Tr_VlanTab $chip $unit 0 1 1
    return PASS
}


