xuiIncludeJSFile("/scripts/global.js");
function wlan_ClientTriangulation_SubmitStart(submitFlag)
{
}
function wlan_ClientTriangulation_SubmitEnd(submitFlag)
{
}

function wlan_ClientTriangulation_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Detected Client Status","DetectedClientStatus.html"], ["Rogue Classification","DetectedClientRogueClassification.html"], ["Pre-Auth History","PreAuthenticationHistory.html"],["Triangulation","ClientTriangulation.html"], ["Roam History","RoamHistory.html"] ];
}

function wlan_ClientTriangulation_LoadEnd(arg)
{
  loadme();
}

