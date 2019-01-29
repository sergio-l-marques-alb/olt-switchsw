xuiIncludeJSFile("/scripts/global.js");
function wlan_AdvancedGlobalTab_SubmitStart(submitFlag)
{
}
function wlan_AdvancedGlobalTab_SubmitEnd(submitFlag)
{
}

function wlan_AdvancedGlobalTab_LoadStart(arg)
{
<EMWEB_IF ID=USMWEB_IF_L2_CENTTNNL_FEATURE_PRESENT>
  xeData.nTabInfo = [ ["Global","AdvancedGlobalConfig.html"], ["SNMP Traps","WirelessSNMPTrapsConfig.html"], ["Distributed Tunneling","AdvancedDistTunnelConfig.html"],["L2 Tunneling","cent_tnnl_cfg.html"] ];
<EMWEB_ELSE>
  xeData.nTabInfo = [ ["Global","AdvancedGlobalConfig.html"], ["SNMP Traps","WirelessSNMPTrapsConfig.html"], ["Distributed Tunneling","AdvancedDistTunnelConfig.html"], ["Device Location","DeviceLocation.html"] ];
</EMWEB_IF>
}

function wlan_AdvancedGlobalTab_LoadEnd(arg)
{
  loadme();
}

