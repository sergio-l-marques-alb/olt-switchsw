xuiIncludeJSFile("/scripts/global.js");
function wlan_PreAuthHistorySummary_SubmitStart(submitFlag)
{
}
function wlan_PreAuthHistorySummary_SubmitEnd(submitFlag)
{
}

function wlan_PreAuthHistorySummary_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Detected Client Summary","DetectedClientSummary.html"], ["Pre-Authentication History Summary","PreAuthenticationHistorySummary.html"], ["Roam History Summary","RoamHistorySummary.html"] ];
}

function wlan_PreAuthHistorySummary_LoadEnd(arg)
{
  loadme();
}

