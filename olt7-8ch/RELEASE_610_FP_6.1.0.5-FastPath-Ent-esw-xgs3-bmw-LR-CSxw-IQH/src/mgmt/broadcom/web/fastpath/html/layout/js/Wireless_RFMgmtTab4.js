xuiIncludeJSFile("/scripts/global.js");
function Wireless_RFMgmtTab4_SubmitStart(submitFlag)
{
}
function Wireless_RFMgmtTab4_SubmitEnd(submitFlag)
{
}

function Wireless_RFMgmtTab4_LoadStart(arg)
{
  xeData.nTabInfo = [["Configuration","RFConfiguration.html"],["Channel Plan History","ChannelPlanHistory.html"],["Manual Channel Plan","ManualChannelPlan.html"],["Manual Power Adjustments","ManualPowerAdjustments.html"]];
}

function Wireless_RFMgmtTab4_LoadEnd(arg)
{
  loadme();
}

