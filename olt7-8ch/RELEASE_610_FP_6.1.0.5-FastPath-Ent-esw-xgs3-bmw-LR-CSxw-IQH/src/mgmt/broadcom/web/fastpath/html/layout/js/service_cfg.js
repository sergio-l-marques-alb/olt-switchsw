function service_cfg_SubmitStart(submitFlag)
{
  var obj_xids= new Array("1_12_1","1_5_1","1_2_1");
  var idx=0;
  while (idx <= 2)
  {
    queueObj = getInputElementByXid(obj_xids[idx]);
    if (queueObj && queueObj.nextSibling && queueObj.nextSibling.value == "")
    {
      queueObj.disabled = true;
      queueObj.nextSibling.disabled = true;
    }
    idx++;
  }
}
function service_cfg_SubmitEnd(submitFlag)
{
}

function service_cfg_LoadStart(arg)
{
  var objArr = new Array("2_5_1", "2_8_1","4_7_1","4_8_1");
  var idx=0;
  while (idx < objArr.length)
  {
    var obj = getInputElementByXid(objArr[idx]);
    if (obj && obj.value)
     obj.value =  obj.value.replace(/\n/g,"<BR>");
    idx++;
  }
}

function service_cfg_LoadEnd(arg)
{
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
