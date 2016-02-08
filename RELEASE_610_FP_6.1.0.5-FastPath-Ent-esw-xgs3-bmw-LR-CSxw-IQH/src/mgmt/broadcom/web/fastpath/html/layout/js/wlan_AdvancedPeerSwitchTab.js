xuiIncludeJSFile("/scripts/global.js");
function wlan_AdvancedPeerSwitchTab_SubmitStart(submitFlag)
{
}
function wlan_AdvancedPeerSwitchTab_SubmitEnd(submitFlag)
{
}

function wlan_AdvancedPeerSwitchTab_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Configuration Request","PeerSwitchConfigRequest.html"], ["Configuration Enable/Disable","PeerSwitchConfigEnableDisable.html"], ["Mutual Authentication","PeerSwitchConfigMutualAuthentication.html"] ];
}

function wlan_AdvancedPeerSwitchTab_LoadEnd(arg)
{
  loadme();
}

