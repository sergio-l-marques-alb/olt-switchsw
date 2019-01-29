xuiIncludeJSFile("/scripts/global.js");
function wlan_DetectedClientStatus_SubmitStart(submitFlag)
{
}
function wlan_DetectedClientStatus_SubmitEnd(submitFlag)
{
}

function wlan_DetectedClientStatus_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Detected Client Status","client_detected_status.html"], ["Rogue Classification","DetectedClientRogueClassification.html"], ["Pre-Auth History","client_pre_auth_history.html"],["Triangulation","ClientTriangulation.html"], ["Roam History","client_roam_history.html"] ];
}

function wlan_DetectedClientStatus_LoadEnd(arg)
{
  loadme();
}

