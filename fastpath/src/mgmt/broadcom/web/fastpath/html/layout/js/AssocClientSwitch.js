xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/AssocClientStatusStatsTabsList.js");
function AssocClientSwitch_SubmitStart()
{
}


function AssocClientSwitch_SubmitEnd()
{
}

function AssocClientSwitch_LoadStart()
{
  xeData.nTabInfo = tabsList;
}

function AssocClientSwitch_LoadEnd()
{
  loadme();
}

