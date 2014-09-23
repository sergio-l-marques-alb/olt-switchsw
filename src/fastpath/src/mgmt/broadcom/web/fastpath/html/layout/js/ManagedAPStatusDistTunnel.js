xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/ManagedAPStatusStatsTabsList.js");
function ManagedAPStatusDistTunnel_SubmitStart()
{
}


function ManagedAPStatusDistTunnel_SubmitEnd()
{
}

function ManagedAPStatusDistTunnel_LoadStart()
{
  xeData.nTabInfo = tabsList;
}

function ManagedAPStatusDistTunnel_LoadEnd()
{
  loadme();
}

