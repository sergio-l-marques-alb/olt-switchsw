xuiIncludeJSFile("/scripts/global.js");
function wlan_RoamHistory_SubmitStart(submitFlag)
{
}
function wlan_RoamHistory_SubmitEnd(submitFlag)
{
}

function wlan_RoamHistory_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Detected Client Status","DetectedClientStatus.html"], ["Rogue Classification","DetectedClientRogueClassification.html"], ["Pre-Auth History","PreAuthenticationHistory.html"],["Triangulation","ClientTriangulation.html"], ["Roam History","RoamHistory.html"] ];
}

function wlan_RoamHistory_LoadEnd(arg)
{
  loadme();
}

