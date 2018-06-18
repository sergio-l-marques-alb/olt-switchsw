xuiIncludeJSFile("/scripts/global.js");
function wlan_RRMNeighborTab_SubmitStart(submitFlag)
{
}
function wlan_RRMNeighborTab_SubmitEnd(submitFlag)
{
}

function wlan_RRMNeighborTab_LoadStart(arg)
{

  xeData.nTabInfo = [ ["RRM Channel Load Configuration","RRMChannelLoadConfig.html"], ["RRM Channel Load History","RRMChannelLoadHistory.html"], ["RRM Neighbors","RRMNeighbor.html"] ];
}

function wlan_RRMNeighborTab_LoadEnd(arg)
{
  loadme();
}

