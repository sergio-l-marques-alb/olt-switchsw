xuiIncludeJSFile("/scripts/global.js");
function wlan_GlobalStatsTab2_SubmitStart(submitFlag)
{
}
function wlan_GlobalStatsTab2_SubmitEnd(submitFlag)
{
}

function wlan_GlobalStatsTab2_LoadStart(arg)
{
<EMWEB_IF ID=USMWEB_IF_AP_IMAGE_AVAILABILITY_PRESENT>
  xeData.nTabInfo = [ ["Global","WirelessGlobalStats.html"], ["Switch Status","wireless_switch_status.html"], ["IP Discovery","WirelessDiscoveryStatus.html"], ["Configuration Received","ConfigurationRecvd.html"], ["AP Hardware Capability",["Summary","APHardwareCapability.html"],["Radio Detail","APHardwareRadioCapability.html"],["Image Table","APHardwareImageCapabilityTable.html"]],["AP Image Availability","wireless_ap_image_version.html"] ];
<EMWEB_ELSE>
  xeData.nTabInfo = [ ["Global","WirelessGlobalStats.html"], ["Switch Status","wireless_switch_status.html"], ["IP Discovery","WirelessDiscoveryStatus.html"], ["Configuration Received","ConfigurationRecvd.html"], ["AP Hardware Capability",["Summary","APHardwareCapability.html"],["Radio Detail","APHardwareRadioCapability.html"],["Image Table","APHardwareImageCapabilityTable.html"]] ];
</EMWEB_IF>
}

function wlan_GlobalStatsTab2_LoadEnd(arg)
{
  loadme();
}

