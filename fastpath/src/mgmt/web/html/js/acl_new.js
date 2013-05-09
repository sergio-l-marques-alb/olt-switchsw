
function acl_new_SubmitStart(submitFlag)
{
  var arr_1_5_1 = getInputElementsByXid('1_5_1');
  var arr_1_5_2 = getInputElementsByXid('1_5_2'); 
  var arr_1_5_3 = getInputElementsByXid('1_5_3');
  var arr_1_6_1 = getInputElementsByXid('1_6_1');
  var arr_1_6_2 = getInputElementsByXid('1_6_2');
  var arr_1_6_3 = getInputElementsByXid('1_6_3');
  var arr_1_3_2 = getInputElementByXid('1_3_2');
  var arr_1_8_1 = getInputElementsByXid('1_8_1'); 
  var arr_1_8_2 = getInputElementsByXid('1_8_2');
if( arr_1_3_2.value == "IP ACL")
{
  for(var index=0;index<arr_1_5_1.length;index++){
  if((arr_1_5_1[index].nextSibling.value!="")&&(arr_1_5_2[index].nextSibling.value!="")&&(arr_1_5_3[index].nextSibling.value=="" ))
  {
    arr_1_5_3[index].disabled =  arr_1_5_3[index].nextSibling.disabled = true;
  }
  else if((arr_1_5_1[index].nextSibling.value=="")||(arr_1_5_2[index].nextSibling.value=="")&&((arr_1_5_3[index].value!="")||(arr_1_5_3[index].nextSibling.value!="")))
  {
   arr_1_5_3[index].disabled =  arr_1_5_3[index].nextSibling.disabled= true;
  }
  else
  {
    arr_1_5_3[index].disabled = arr_1_5_3[index].nextSibling.disabled = false;
  }
 
  if((arr_1_6_1[index].nextSibling.value!="")&&(arr_1_6_2[index].nextSibling.value!="")&&(arr_1_6_3[index].nextSibling.value=="" ))
  {
    arr_1_6_3[index].disabled = arr_1_6_3[index].nextSibling.disabled = true;
  }
  else if((arr_1_6_1[index].nextSibling.value=="")||(arr_1_6_2[index].nextSibling.value=="")&&((arr_1_6_3[index].value!="")||(arr_1_6_3[index].nextSibling.value!="")))
  {
   arr_1_6_3[index].disabled = arr_1_6_3[index].nextSibling.disabled = true;
  }
  else
  {
    arr_1_6_3[index].disabled = arr_1_6_3[index].nextSibling.disabled = false;
  }
    if(((arr_1_5_1[index].nextSibling.value!="")&&(arr_1_5_2[index].nextSibling.value!=""))&&(((arr_1_6_1[index].nextSibling.value=="" )&&(arr_1_6_1[index].value=="" )) && ((arr_1_6_2[index].nextSibling.value=="" )&&(arr_1_6_2[index].value=="" ))))
  {
    arr_1_6_1[index].disabled = arr_1_6_1[index].nextSibling.disabled = true;
    arr_1_6_2[index].disabled = arr_1_6_2[index].nextSibling.disabled = true;
  }
   else if(((arr_1_6_1[index].nextSibling.value!="")&&(arr_1_6_2[index].nextSibling.value!=""))&&(((arr_1_5_1[index].nextSibling.value=="" )&&(arr_1_5_1[index].value=="" )) && ((arr_1_5_2[index].nextSibling.value=="" )&&(arr_1_5_2[index].value=="" ))))
  {
    arr_1_5_1[index].disabled = arr_1_5_1[index].nextSibling.disabled = true;
    arr_1_5_2[index].disabled = arr_1_5_2[index].nextSibling.disabled = true;
  }
 
  continue;
}
}
else if (arr_1_3_2.value == "MAC ACL")
{
    for(var index=0;index<arr_1_8_1.length;index++){
    if((arr_1_8_1[index].nextSibling.value!="") && ((arr_1_8_2[index].nextSibling.value=="" )&&(arr_1_8_2[index].value=="" )))
  {
    arr_1_8_2[index].disabled = arr_1_8_2[index].nextSibling.disabled = true;
  }
   else if((arr_1_8_2[index].nextSibling.value!="")&& ((arr_1_8_1[index].nextSibling.value=="" )&&(arr_1_8_1[index].value=="" )))
  {
    arr_1_8_1[index].disabled = arr_1_8_1[index].nextSibling.disabled = true;
  }
   continue;
 }
}
  
}
  





function acl_new_SubmitEnd(submitFlag){

  

}



function acl_new_LoadStart(arg){

   

}



function acl_new_LoadEnd(arg){

 

}

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

function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){

        var element = all_elements[index];

        if( element.getAttribute('xid') == null ||

            element.getAttribute('xid') != xid) continue;

        return element;

    }

}

