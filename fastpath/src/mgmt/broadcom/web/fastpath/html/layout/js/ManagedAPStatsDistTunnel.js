xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/ManagedAPStatusStatsTabsList.js");
function ManagedAPStatsDistTunnel_SubmitStart()
{
}


function ManagedAPStatsDistTunnel_SubmitEnd()
{
}

function ManagedAPStatsDistTunnel_LoadStart()
{
  xeData.nTabInfo = tabsList;
}

function ManagedAPStatsDistTunnel_LoadEnd()
{
  loadme();
}

