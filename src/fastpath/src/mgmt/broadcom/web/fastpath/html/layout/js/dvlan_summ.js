function dvlan_summ_SubmitStart(submitFlag)
{
}
function dvlan_summ_SubmitEnd(submitFlag)
{
}

function dvlan_summ_LoadStart(arg)
{
 var tblObj = document.getElementsByTagName('table');
 var allRowsOfDVLANtable =  tblObj[1].getElementsByTagName('tr');
 for (var i = 1; i < allRowsOfDVLANtable.length; i++)
 {
    var tmpInputs = allRowsOfDVLANtable[i].getElementsByTagName('input');
    var mode = tmpInputs[2].value;
    
    if(mode != "33024" && mode != "34984")
    {
       mode = "Custom "+"(" +mode+ ")";
    }
    else if(mode == "33024")
    {
      mode = "802.1Q";
    }  
    else if(mode == "34984")
    {
      mode = "vMAN";
    }
    tmpInputs[2].value = mode;
 } 
}

function dvlan_summ_LoadEnd(arg)
{
}

