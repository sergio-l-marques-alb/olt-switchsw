xuiIncludeJSFile("/scripts/global.js");
function wlan_RogueRFScanTab_SubmitStart(submitFlag)
{
}
function wlan_RogueRFScanTab_SubmitEnd(submitFlag)
{
}

function wlan_RogueRFScanTab_LoadStart(arg)
{

  xeData.nTabInfo = [ ["AP RF Scan Status","managed_aprfscan_status.html"], ["AP Triangulation Status","APTriangulationStatus.html"], ["WIDS AP Rogue Classification","APRogueClassification.html"] ];
}

function wlan_RogueRFScanTab_LoadEnd(arg)
{
  loadme();
}

