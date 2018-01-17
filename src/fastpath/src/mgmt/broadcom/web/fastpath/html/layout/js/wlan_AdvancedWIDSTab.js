xuiIncludeJSFile("/scripts/global.js");
function wlan_AdvancedWIDSTab_SubmitStart(submitFlag)
{
}
function wlan_AdvancedWIDSTab_SubmitEnd(submitFlag)
{
}

function wlan_AdvancedWIDSTab_LoadStart(arg)
{

  xeData.nTabInfo = [ ["AP Configuration","widsAPConfig.html"], ["Client Configuration","widsClientConfig.html"] ];
}

function wlan_AdvancedWIDSTab_LoadEnd(arg)
{
  loadme();
}

