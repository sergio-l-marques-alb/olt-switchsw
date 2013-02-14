xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/AssocClientStatusStatsTabsList.js");
function AssocClientStatusTspec_SubmitStart()
{
}


function AssocClientStatusTspec_SubmitEnd()
{
}

function AssocClientStatusTspec_LoadStart()
{
  xeData.nTabInfo = tabsList;
}

function AssocClientStatusTspec_LoadEnd()
{
  loadme();
}

