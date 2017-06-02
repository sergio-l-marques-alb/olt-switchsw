xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/ManagedAPStatusStatsTabsList.js");
function ManagedAPStatusVAP_SubmitStart()
{
}


function ManagedAPStatusVAP_SubmitEnd()
{
}

function ManagedAPStatusVAP_LoadStart()
{
  xeData.nTabInfo = tabsList;
}

function ManagedAPStatusVAP_LoadEnd()
{
  loadme();
}

