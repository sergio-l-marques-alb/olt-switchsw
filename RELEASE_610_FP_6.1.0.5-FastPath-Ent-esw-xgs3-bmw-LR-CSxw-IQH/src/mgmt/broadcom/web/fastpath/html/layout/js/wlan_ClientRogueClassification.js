xuiIncludeJSFile("/scripts/global.js");
function wlan_ClientRogueClassification_SubmitStart(submitFlag)
{
}
function wlan_ClientRogueClassification_SubmitEnd(submitFlag)
{
}

function wlan_ClientRogueClassification_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Detected Client Status","DetectedClientStatus.html"], ["Rogue Classification","DetectedClientRogueClassification.html"], ["Pre-Auth History","PreAuthenticationHistory.html"],["Triangulation","ClientTriangulation.html"], ["Roam History","RoamHistory.html"] ];
}

function wlan_ClientRogueClassification_LoadEnd(arg)
{
  loadme();
}

