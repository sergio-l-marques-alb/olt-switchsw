xuiIncludeJSFile("/scripts/global.js");
function wlan_RRMChannelLoadHistoryTab_SubmitStart(submitFlag)
{
}
function wlan_RRMChannelLoadHistoryTab_SubmitEnd(submitFlag)
{
}

function wlan_RRMChannelLoadHistoryTab_LoadStart(arg)
{

  xeData.nTabInfo = [ ["RRM Channel Load Configuration","RRMChannelLoadConfig.html"], ["RRM Channel Load History","RRMChannelLoadHistory.html"], ["RRM Neighbors","RRMNeighbor.html"] ]

  var inputDIV = "pagination_summary";
  var outputDIV = "plotpagination_summary";

  var apSummaryTableRepeatInfo = xuiRepeatInfoReadByCount(inputDIV, 7);
  if(apSummaryTableRepeatInfo.rows !=0 ){
     xui_JSPagination(apSummaryTableRepeatInfo,outputDIV, '2_1');
  }

}

function wlan_RRMChannelLoadHistoryTab_LoadEnd(arg)
{
  loadme();
}


