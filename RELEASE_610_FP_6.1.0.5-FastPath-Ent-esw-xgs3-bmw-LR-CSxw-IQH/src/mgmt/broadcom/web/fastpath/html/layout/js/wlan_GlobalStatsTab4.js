xuiIncludeJSFile("/scripts/global.js");
function wlan_GlobalStatsTab4_SubmitStart(submitFlag)
{
}
function wlan_GlobalStatsTab4_SubmitEnd(submitFlag)
{
}

function wlan_GlobalStatsTab4_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Global","WirelessGlobalStats.html"], ["Switch Status","SwitchStatusStats.html"], ["IP Discovery","WirelessDiscoveryStatus.html"], ["Configuration Received","ConfigurationRecvd.html"], ["AP Hardware Capability",["Summary","APHardwareCapability.html"],["Radio Detail","APHardwareRadioCapability.html"],["Image Table","APHardwareImageCapabilityTable.html"]] ];
}

function wlan_GlobalStatsTab4_LoadEnd(arg)
{
  loadme();
}

