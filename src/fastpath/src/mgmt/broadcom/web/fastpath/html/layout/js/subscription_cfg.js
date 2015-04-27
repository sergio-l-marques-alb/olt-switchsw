function subscription_cfg_SubmitStart(submitFlag)
{
  var nniInterfaceList = getInputElementByXid('4_10_1');
}
function subscription_cfg_SubmitEnd(submitFlag)
{
  var nniInterfaceList = getInputElementByXid('4_10_1');
  if(nniInterfaceList.value == "")
  {
    nniInterfaceList.disabled=true;
  }   
}

function subscription_cfg_LoadStart(arg)
{
}

function subscription_cfg_LoadEnd(arg)
{
//  var element_1_5_1 = getInputElementByXid('1_5_1');
  var nniInterfaceList = getInputElementByXid('4_10_1');

     //  if(element_1_5_1 == null || element_1_5_1.nextSibling == null )
       //   return;
       if(nniInterfaceList == null || nniInterfaceList.nextSibling == null )
          return;

//     element_1_5_1 = getInputElementByXid('1_5_1').nextSibling;
  //   element_1_5_1.size = 8;
    // element_1_5_1.readOnly = false;
    // element_1_5_1.overflow = scroll;
    // element_1_5_1.disabled = false;

     nniInterfaceList = getInputElementByXid('4_10_1').nextSibling;
     nniInterfaceList.size = 8;
     nniInterfaceList.readOnly = true;
     nniInterfaceList.overflow = scroll;
     nniInterfaceList.disabled = false;
}


function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }


}
