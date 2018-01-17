xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/ManagedAPStatusStatsTabsList.js");
function ManagedAPStatsRadio_SubmitStart()
{
}


function ManagedAPStatsRadio_SubmitEnd()
{
}

function ManagedAPStatsRadio_LoadStart()
{
  xeData.nTabInfo = tabsList;
}

function ManagedAPStatsRadio_LoadEnd()
{
  loadme();
}

