xuiIncludeJSFile("/scripts/global.js");
function wlan_StatisticsWDSStatusTab_SubmitStart(submitFlag)
{
}
function wlan_StatisticsWDSStatusTab_SubmitEnd(submitFlag)
{
}

function wlan_StatisticsWDSStatusTab_LoadStart(arg)
{

  xeData.nTabInfo = [ ["WDS AP Group Status Summary","wds_group_summary.html"], ["WDS AP Group Status","WdsGroupStatus.html"], ["WDS AP Status","wds_ap_status.html"], ["WDS Link Status Summary","wds_link_status.html"], ["WDS Link Statistics Summary","wds_link_statistics.html"] ];
}

function wlan_StatisticsWDSStatusTab_LoadEnd(arg)
{
  loadme();
}

