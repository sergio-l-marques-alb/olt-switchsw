xuiIncludeJSFile("/scripts/global.js");

function LocationTriggerGlobalStatus_SubmitStart(submitFlag)
{
}

function LocationTriggerGlobalStatus_SubmitEnd(submitFlag)
{
}

function LocationTriggerGlobalStatus_LoadStart(arg)
{
  xeData.nTabInfo = [ ["Global Status","LocationTriggerGlobalStatus.html"], ["Floor Status","LocationTriggerFloorStatus.html"] ];
}

function LocationTriggerGlobalStatus_LoadEnd(arg)
{
  loadme();
}
