xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/AssocClientStatusStatsTabsList.js");
function AssocClientStatusRRM_SubmitStart()
{
}


function AssocClientStatusRRM_SubmitEnd()
{
}

function AssocClientStatusRRM_LoadStart()
{
  xeData.nTabInfo = tabsList;
}

function AssocClientStatusRRM_LoadEnd()
{
  loadme();
}

