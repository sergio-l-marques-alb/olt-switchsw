function poe_SubmitStart(submitFlag)
{
}
function poe_SubmitEnd(submitFlag)
{
}

function poe_LoadStart(arg)
{
  var arr_2_1_2 = getInputElementsByXid('2_1_2');
  
  for(var index=0;index<arr_2_1_2.length;index++){
      if(arr_2_1_2[index].value == "Class0" )
      {
         arr_2_1_2[index].value = "0" ;
      } 
      else if(arr_2_1_2[index].value == "Class1" )
      {
          arr_2_1_2[index].value = "1" ;    
      }
      else if(arr_2_1_2[index].value == "Class2" )
      {
          arr_2_1_2[index].value = "2" ;
      }
      else if(arr_2_1_2[index].value == "Class3" )
      {
          arr_2_1_2[index].value = "3" ;
      }
      else if(arr_2_1_2[index].value == "Class4" )
      {
          arr_2_1_2[index].value = "4" ;
      }
      else 
      {
          arr_2_1_2[index].value = "" ;
      }

   }
}


function poe_LoadEnd(arg)
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

