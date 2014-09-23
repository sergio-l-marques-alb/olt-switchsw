
function voicevlan_SubmitStart(submitFlag){
var arr_2_1_2 = getInputElementsByXid('2_1_2');
var arr_2_1_4 = getInputElementsByXid('2_1_4');
var arr_2_1_5 = getInputElementsByXid('2_1_5');
for(var index=0;index<arr_2_1_2.length;index++){   
 var selected_value = arr_2_1_2[index].nextSibling.options[arr_2_1_2[index].nextSibling.selectedIndex].innerHTML;
 if(selected_value == "Voice VLANID")
 {
  arr_2_1_5[index].nextSibling.disabled =  arr_2_1_5[index].disabled = true;
  if(arr_2_1_4[index].value != "" || arr_2_1_4[index].nextSibling.value != "") 
  {
   arr_2_1_4[index].nextSibling.disabled =arr_2_1_4[index].disabled= false; 
  }
  else
  {
   arr_2_1_4[index].nextSibling.disabled =arr_2_1_4[index].disabled= true;
  }
 }
 else if(selected_value == "Priority")
 {
  arr_2_1_4[index].nextSibling.disabled = arr_2_1_4[index].disabled = true;
  if(arr_2_1_5[index].value != "" || arr_2_1_5[index].nextSibling.value != "")
  {
   arr_2_1_5[index].nextSibling.disabled = arr_2_1_5[index].disabled = false;
  }
  else 
  {
    arr_2_1_5[index].nextSibling.disabled = arr_2_1_5[index].disabled = true;
  }
}
 else if(selected_value == "None")
 {
   arr_2_1_4[index].nextSibling.disabled = arr_2_1_4[index].disabled = true;
   arr_2_1_5[index].nextSibling.disabled =  arr_2_1_5[index].disabled = true;

 }
 else if(selected_value == "Untagged")
 {
   arr_2_1_4[index].nextSibling.disabled = arr_2_1_4[index].disabled = true;
   arr_2_1_5[index].nextSibling.disabled =  arr_2_1_5[index].disabled = true;

 }
 else
 {
   arr_2_1_4[index].nextSibling.disabled = arr_2_1_4[index].disabled = true;
   arr_2_1_5[index].nextSibling.disabled =  arr_2_1_5[index].disabled = true;
 }
 continue;
}
}





function voicevlan_SubmitEnd(submitFlag){

  

}



function voicevlan_LoadStart(arg){
  var arr_2_1_4 = getInputElementsByXid('2_1_4');
  var arr_2_1_5 = getInputElementsByXid('2_1_5');
  for(var index=0;index<arr_2_1_4.length;index++){
      if(arr_2_1_4[index].value == "0" )
      {
         arr_2_1_4[index].value = "" ;
      }
      if(arr_2_1_5[index].value == "255" )
      {
         arr_2_1_5[index].value = "" ;
      }

      continue;
   }
}



function voicevlan_LoadEnd(arg){

 

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

