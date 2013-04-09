xuiIncludeJSFile("/scripts/global.js");
function Wireless_RFMgmtTab2_SubmitStart(submitFlag)
{
}
function Wireless_RFMgmtTab2_SubmitEnd(submitFlag)
{
}

function Wireless_RFMgmtTab2_LoadStart(arg)
{
  xeData.nTabInfo = [["Configuration","channel_pwr_cfg.html"],["Channel Plan History","ChannelPlanHistory.html"],["Manual Channel Plan","man_channel_status.html"],["Manual Power Adjustments","ManualPowerAdjustments.html"]];
}

function Wireless_RFMgmtTab2_LoadEnd(arg)
{
  loadme();
}

