xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/AssocClientStatusStatsTabsList.js");
function AssocClientStatusQoS_SubmitStart()
{
}


function AssocClientStatusQoS_SubmitEnd()
{
}

function AssocClientStatusQoS_LoadStart()
{
  xeData.nTabInfo = tabsList;
}

function AssocClientStatusQoS_LoadEnd()
{
  loadme();
}

