function wlan_networksumm_SubmitStart(submitFlag)
{
}
function wlan_networksumm_SubmitEnd(submitFlag)
{
}

function wlan_networksumm_LoadStart(arg)
{
}

function wlan_networksumm_LoadEnd(arg)
{
  var tblObj = document.getElementsByTagName('table');
  var allRowsOfWLANNetworktable =  tblObj[1].getElementsByTagName('tr');
  for (var i = 1; i < allRowsOfWLANNetworktable.length; i++)
  {
    var tmpInputs = allRowsOfWLANNetworktable[i].getElementsByTagName('input');
    var temp = tmpInputs[2].value;
    var networkId = parseInt(temp);
    if(networkId > 16)
    {
      tmpInputs[0].nextSibling.disabled = false;
    }
    else
    {
      tmpInputs[0].nextSibling.disabled = true;
    }
  }
 
  var addBtn = document.getElementById("2_2_2");
  if(addBtn != null)
  {
    addBtn.align = "left";  
  }
}

