xuiIncludeJSFile("/scripts/global.js");
function wlan_GlobalStatsTab7_SubmitStart(submitFlag)
{
}
function wlan_GlobalStatsTab7_SubmitEnd(submitFlag)
{
}

function wlan_GlobalStatsTab7_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Global","WirelessGlobalStats.html"], ["Switch Status","SwitchStatusStats.html"], ["IP Discovery","WirelessDiscoveryStatus.html"], ["Configuration Received","ConfigurationRecvd.html"], ["AP Hardware Capability",["Summary","APHardwareCapability.html"],["Radio Detail","APHardwareRadioCapability.html"],["Image Table","APHardwareImageCapabilityTable.html"]] ];
}

function wlan_GlobalStatsTab7_LoadEnd(arg)
{
  loadme();
}

