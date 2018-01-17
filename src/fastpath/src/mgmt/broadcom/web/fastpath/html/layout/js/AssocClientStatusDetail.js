xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/AssocClientStatusStatsTabsList.js");
function AssocClientStatusDetail_SubmitStart()
{
}


function AssocClientStatusDetail_SubmitEnd()
{
}

function AssocClientStatusDetail_LoadStart()
{
  xeData.nTabInfo = tabsList;
}

function AssocClientStatusDetail_LoadEnd()
{
  loadme();
}

