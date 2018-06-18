xuiIncludeJSFile("/scripts/global.js");
function Wireless_RFMgmtTab3_SubmitStart(submitFlag)
{
}
function Wireless_RFMgmtTab3_SubmitEnd(submitFlag)
{
}

function Wireless_RFMgmtTab3_LoadStart(arg)
{
  xeData.nTabInfo = [["Configuration","channel_pwr_cfg.html"],["Channel Plan History","ChannelPlanHistory.html"],["Manual Channel Plan","ManualChannelPlan.html"],["Manual Power Adjustments","ManualPowerAdjustments.html"]];
}

function Wireless_RFMgmtTab3_LoadEnd(arg)
{
  loadme();
}

