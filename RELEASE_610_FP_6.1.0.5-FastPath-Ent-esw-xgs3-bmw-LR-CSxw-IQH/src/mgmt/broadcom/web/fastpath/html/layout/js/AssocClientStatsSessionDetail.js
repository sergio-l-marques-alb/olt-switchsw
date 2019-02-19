xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/AssocClientStatusStatsTabsList.js");
function AssocClientStatsSessionDetail_SubmitStart()
{
}


function AssocClientStatsSessionDetail_SubmitEnd()
{
}

function AssocClientStatsSessionDetail_LoadStart()
{
  xeData.nTabInfo = tabsList;
}

function AssocClientStatsSessionDetail_LoadEnd()
{
  loadme();
}

