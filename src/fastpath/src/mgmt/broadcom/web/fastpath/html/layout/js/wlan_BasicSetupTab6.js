xuiIncludeJSFile("/scripts/global.js");
function wlan_BasicSetupTab6_SubmitStart(submitFlag)
{
}
function wlan_BasicSetupTab6_SubmitEnd(submitFlag)
{
}

function wlan_BasicSetupTab6_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Global","WirelessGlobalConfig.html"], ["Discovery","WirelessDiscoveryConfig.html"], ["Profile","WirelessDefaultProfileConfig.html"], ["Radio","profile_radio_cfg_rw.html"], ["VAP","profile_vap_summ.html"], ["Valid AP","local_ap_database_summ.html"] ];
}

function wlan_BasicSetupTab6_LoadEnd(arg)
{
  loadme();
}

