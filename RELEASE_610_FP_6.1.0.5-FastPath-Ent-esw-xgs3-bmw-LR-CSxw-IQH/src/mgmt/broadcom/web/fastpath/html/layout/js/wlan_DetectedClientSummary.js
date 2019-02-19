xuiIncludeJSFile("/scripts/global.js");
function wlan_DetectedClientSummary_SubmitStart(submitFlag)
{
}
function wlan_DetectedClientSummary_SubmitEnd(submitFlag)
{
}

function wlan_DetectedClientSummary_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Detected Client Summary","DetectedClientSummary.html"], ["Pre-Authentication History Summary","PreAuthenticationHistorySummary.html"], ["Roam History Summary","RoamHistorySummary.html"] ];
}

function wlan_DetectedClientSummary_LoadEnd(arg)
{
  loadme();
}

