xuiIncludeJSFile("/scripts/global.js");
function wlan_PreAuthHistory_SubmitStart(submitFlag)
{
}
function wlan_PreAuthHistory_SubmitEnd(submitFlag)
{
}

function wlan_PreAuthHistory_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Detected Client Status","DetectedClientStatus.html"], ["Rogue Classification","DetectedClientRogueClassification.html"], ["Pre-Auth History","PreAuthenticationHistory.html"],["Triangulation","ClientTriangulation.html"], ["Roam History","RoamHistory.html"] ];
}

function wlan_PreAuthHistory_LoadEnd(arg)
{
  loadme();
}

