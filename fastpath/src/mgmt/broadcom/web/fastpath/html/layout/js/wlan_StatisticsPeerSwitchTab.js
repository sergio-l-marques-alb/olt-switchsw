xuiIncludeJSFile("/scripts/global.js");
function wlan_StatisticsPeerSwitchTab_SubmitStart(submitFlag)
{
}
function wlan_StatisticsPeerSwitchTab_SubmitEnd(submitFlag)
{
}

function wlan_StatisticsPeerSwitchTab_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Status","PeerSwitchStatus.html"], ["Configuration","PeerSwitchConfigStatus.html"], ["Managed AP","PeerSwitchManagedAPStatus.html"] ];
}

function wlan_StatisticsPeerSwitchTab_LoadEnd(arg)
{
  loadme();
}

