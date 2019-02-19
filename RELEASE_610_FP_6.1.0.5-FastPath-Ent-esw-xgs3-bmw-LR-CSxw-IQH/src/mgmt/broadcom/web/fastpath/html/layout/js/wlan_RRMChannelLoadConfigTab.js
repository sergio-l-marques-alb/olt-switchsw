xuiIncludeJSFile("/scripts/global.js");
function wlan_RRMChannelLoadConfigTab_SubmitStart(submitFlag)
{
}
function wlan_RRMChannelLoadConfigTab_SubmitEnd(submitFlag)
{
}

function wlan_RRMChannelLoadConfigTab_LoadStart(arg)
{

  xeData.nTabInfo = [ ["RRM Channel Load Configuration","RRMChannelLoadConfig.html"], ["RRM Channel Load History","rrm_chload_history_summ.html"], ["RRM Neighbors","rrm_neighbors_summ.html"] ];
}

function wlan_RRMChannelLoadConfigTab_LoadEnd(arg)
{
  loadme();
}


