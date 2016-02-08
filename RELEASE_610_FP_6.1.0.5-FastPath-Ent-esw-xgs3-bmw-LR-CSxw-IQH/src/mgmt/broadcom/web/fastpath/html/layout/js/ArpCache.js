
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

function ArpCache_SubmitStart(submitFlag){

}


function ArpCache_SubmitEnd(submitFlag){

}

function ArpCache_LoadEnd(arg){

}

function ArpCache_LoadStart(arg)
{
  var interfaces_1_1_4 = getInputElementsByXid('1_1_4');

  for (var idx=0; idx<interfaces_1_1_4.length; idx++)
  {
    if (interfaces_1_1_4[idx].value == "")
    {
      interfaces_1_1_4[idx].parentNode.innerHTML+= "Management";
    }
  }
}
