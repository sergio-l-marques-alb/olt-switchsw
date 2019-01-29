
function getInputElementsByXid(xid)
{
    var all_elements = document.getElementsByTagName('input') ;
    var result_array = new Array();

    for(var index=0;index<all_elements.length;index++)
    {
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        result_array.push(element);
    }

    return result_array;
}

function DHCPSnoopingInterfaceConfig_SubmitStart(submitFlag){
  var rateLimit = getInputElementsByXid('1_5_1');
  var checkBx = getInputElementsByXid('1_5_4');
  var burstInterval = getInputElementsByXid('1_4_1');

  if (rateLimit[0].value == "-1" && checkBx[0].nextSibling.checked == true)
  {
    rateLimit[0].disabled = "disabled";
    rateLimit[0].nextSibling.disabled = "disabled";
    burstInterval[0].disabled = "disabled";
    burstInterval[0].nextSibling.disabled = "disabled";
  }
}


function DHCPSnoopingInterfaceConfig_SubmitEnd(submitFlag){

}

function DHCPSnoopingInterfaceConfig_LoadEnd(arg){
  var rateLimit = getInputElementsByXid('1_5_1');
  var checkBx = getInputElementsByXid('1_5_4');
  var burstInterval = getInputElementsByXid('1_4_1');

  if (rateLimit[0].value == "-1" && checkBx[0].nextSibling.checked == true)
  {
    rateLimit[0].nextSibling.disabled = "disabled";
    burstInterval[0].nextSibling.disabled = "disabled";
  }
}

function DHCPSnoopingInterfaceConfig_LoadStart(arg)
{

}
