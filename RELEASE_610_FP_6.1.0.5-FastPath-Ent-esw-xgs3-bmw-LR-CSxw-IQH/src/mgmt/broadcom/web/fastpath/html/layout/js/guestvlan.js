function guestvlan_SubmitStart(submitFlag)
{
  var arr_1_1_2 = getInputElementsByXid('1_1_2');
  var arr_1_1_3 = getInputElementsByXid('1_1_3');
  for(var index=0;index<arr_1_1_2.length;index++){      
      var selected_value = arr_1_1_2[index].nextSibling.options[arr_1_1_2[index].nextSibling.selectedIndex].innerHTML;
      if(selected_value == "Remove")
      {
         arr_1_1_3[index].enabled =  arr_1_1_3[index].nextSibling.enabled=true;
		 arr_1_1_3[index].nextSibling.value = "0" ;
	   }
      else if (selected_value == "None")
      {
        arr_1_1_3[index].nextSibling.disabled = arr_1_1_3[index].disabled= true;
      }
      continue;
  }
  for(var index=0;index<arr_1_1_3.length;index++){
  if( arr_1_1_3[index].value == " " ||  arr_1_1_3[index].nextSibling.value == " ")
    {
     arr_1_1_3[index].disabled =  arr_1_1_3[index].nextSibling.disabled= true;
    }
    continue;
  }
}  
function guestvlan_SubmitEnd(submitFlag)
{
}

function guestvlan_LoadStart(arg)
{
  var arr_1_1_3 = getInputElementsByXid('1_1_3');
  
  for(var index=0;index<arr_1_1_3.length;index++){
      if(arr_1_1_3[index].value == "0" )
      {
         arr_1_1_3[index].value = "" ;
      }
      continue; 
   }
}


function guestvlan_LoadEnd(arg)
{
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

