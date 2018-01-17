xuiIncludeJSFile("/scripts/global.js");
function wlan_BasicSetupTab5_SubmitStart(submitFlag)
{
}
function wlan_BasicSetupTab5_SubmitEnd(submitFlag)
{
}

function wlan_BasicSetupTab5_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Global","WirelessGlobalConfig.html"], ["Discovery","WirelessDiscoveryConfig.html"], ["Profile","WirelessDefaultProfileConfig.html"], ["Radio","profile_radio_cfg_rw.html"], ["VAP","profile_vap_summ.html"], ["Valid AP","local_ap_database_summ.html"] ];
}

function wlan_BasicSetupTab5_LoadEnd(arg)
{
  loadme();

  var tblObj = document.getElementsByTagName('table');
  var allRowsOfWLANVAPtable =  tblObj[4].getElementsByTagName('tr');
  for (var i = 1; i < allRowsOfWLANVAPtable.length; i++)
  {
    var tmpInputs = allRowsOfWLANVAPtable[i].getElementsByTagName('input');
    if(i==1)
    {
      tmpInputs[0].nextSibling.checked = true;
      tmpInputs[0].nextSibling.disabled = true;
      tmpInputs[6].nextSibling.disabled = false;
    }
    
  }
}

