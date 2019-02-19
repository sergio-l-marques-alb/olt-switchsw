xuiIncludeJSFile("/scripts/global.js");
function wlan_AdvancedGlobalTab_SubmitStart(submitFlag)
{
}
function wlan_AdvancedGlobalTab_SubmitEnd(submitFlag)
{
}

function wlan_AdvancedGlobalTab_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Global","AdvancedGlobalConfig.html"], ["SNMP Traps","WirelessSNMPTrapsConfig.html"], ["Distributed Tunneling","AdvancedDistTunnelConfig.html"] ];
}

function wlan_AdvancedGlobalTab_LoadEnd(arg)
{
  loadme();
}

