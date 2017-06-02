
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

function DhcpSnoopingPersistentConfig_SubmitStart(submitFlag){

}


function DhcpSnoopingPersistentConfig_SubmitEnd(submitFlag){

}

function DhcpSnoopingPersistentConfig_LoadEnd(arg){
  var remoteFileName = getInputElementsByXid('1_3_1');
  var localRadioButton = getInputElementsByXid('1_1_1');
  var remoteRadioButton = getInputElementsByXid('1_1_2');

  if (remoteFileName[0].value == "")
  {
    remoteRadioButton[0].nextSibling.checked = false;
  }
  else
  {
    remoteRadioButton[0].nextSibling.checked = true;
    localRadioButton[0].nextSibling.checked = false;
  }
}

function DhcpSnoopingPersistentConfig_LoadStart(arg)
{

}
