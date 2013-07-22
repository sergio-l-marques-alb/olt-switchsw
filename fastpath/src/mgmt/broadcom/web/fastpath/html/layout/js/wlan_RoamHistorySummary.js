xuiIncludeJSFile("/scripts/global.js");
function wlan_RoamHistorySummary_SubmitStart(submitFlag)
{
}
function wlan_RoamHistorySummary_SubmitEnd(submitFlag)
{
}

function wlan_RoamHistorySummary_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Detected Client Summary","DetectedClientSummary.html"], ["Pre-Authentication History Summary","PreAuthenticationHistorySummary.html"], ["Roam History Summary","RoamHistorySummary.html"] ];
}

function wlan_RoamHistorySummary_LoadEnd(arg)
{
  loadme();
}

