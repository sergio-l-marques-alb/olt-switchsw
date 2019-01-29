xuiIncludeJSFile("/scripts/global.js");

function LocationTriggerFloorStatus_SubmitStart(submitFlag)
{
}

function LocationTriggerFloorStatus_SubmitEnd(submitFlag)
{
}

function LocationTriggerFloorStatus_LoadStart(arg)
{
  xeData.nTabInfo = [ ["Global Status","LocationTriggerGlobalStatus.html"], ["Floor Status","LocationTriggerFloorStatus.html"] ];
}

function LocationTriggerFloorStatus_LoadEnd(arg)
{
  loadme();
}

