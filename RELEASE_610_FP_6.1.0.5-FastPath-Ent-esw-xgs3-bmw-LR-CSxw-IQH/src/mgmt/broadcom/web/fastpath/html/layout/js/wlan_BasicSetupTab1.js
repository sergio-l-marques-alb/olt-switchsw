xuiIncludeJSFile("/scripts/global.js");
function wlan_BasicSetupTab1_SubmitStart(submitFlag)
{
}
function wlan_BasicSetupTab1_SubmitEnd(submitFlag)
{
}

function wlan_BasicSetupTab1_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Global","WirelessGlobalConfig.html"], ["Discovery","WirelessDiscoveryConfig.html"], ["Profile","WirelessDefaultProfileConfig.html"], ["Radio","WirelessDefaultRadioConfiguration.html"], ["VAP","WirelessDefaultVAPConfig.html"], ["Valid AP","local_ap_database_summ.html"], ["OUI","local_oui_database_summ.html"] ];
}

function wlan_BasicSetupTab1_LoadEnd(arg)
{
  loadme();
}

