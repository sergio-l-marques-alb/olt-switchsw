function CSTPortConfigStatus_SubmitStart(submitFlag)
{
  var elem_1_6_1 = getInputElementByXid("1_6_1");

  if (elem_1_6_1 && elem_1_6_1.nextSibling && elem_1_6_1.nextSibling.value == "Not Configured")
  {
    elem_1_6_1.disabled = true;
    elem_1_6_1.nextSibling.disabled = true;
  }
}

function CSTPortConfigStatus_SubmitEnd(submitFlag)
{
}

function CSTPortConfigStatus_LoadStart(arg)
{
}

function CSTPortConfigStatus_LoadEnd(arg)
{
}


function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        
        if(element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }


}
