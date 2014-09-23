function subsc_cfg_SubmitStart(submitFlag)
{

/* Match CVID      1_5_1  */
/* Match Priority  1_6_1  */
/* New CVID        1_9_1  */
/* Assign Queue    1_11_1 */
/* Data Rate       1_13_1 */
/* Burst Size      1_14_1 */

  var obj_xids= new Array("1_5_1","1_6_1","1_9_1","1_11_1","1_13_1","1_14_1");
  var idx=0;
  while (idx <= 5)
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

function subsc_cfg_SubmitEnd(submitFlag)
{
}

function subsc_cfg_LoadStart(arg)
{
  var objArr = new Array("2_8_1", "4_8_1");
  var idx=0;
  while (idx < objArr.length)
  {
    var obj = getInputElementByXid(objArr[idx]);
    if (obj && obj.value)
     obj.value =  obj.value.replace(/\n/g,"<BR>");
    idx++;
  }
}

function subsc_cfg_LoadEnd(arg)
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

