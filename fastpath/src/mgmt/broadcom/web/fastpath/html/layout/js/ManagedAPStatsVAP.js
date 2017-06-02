xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/ManagedAPStatusStatsTabsList.js");
function ManagedAPStatsVAP_SubmitStart()
{
}


function ManagedAPStatsVAP_SubmitEnd()
{
}

function ManagedAPStatsVAP_LoadStart()
{
  xeData.nTabInfo = tabsList;
}

function ManagedAPStatsVAP_LoadEnd()
{
  loadme();
}

