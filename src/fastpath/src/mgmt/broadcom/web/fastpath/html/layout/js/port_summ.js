function port_summ_SubmitStart(submitFlag)
{
}
function port_summ_SubmitEnd(submitFlag)
{
}

function port_summ_LoadStart(arg)
{
  var arr_1_1_2 = getInputElementsByXid('1_1_2');
  
  for(var index=0;index<arr_1_1_2.length;index++){
      if(arr_1_1_2[index].value == "L7_PHY_CAP_PORTSPEED_SFP" || arr_1_1_2[index].value == "L7_PHY_CAP_PORTSPEED_SFP_DETECT" )
      {
         arr_1_1_2[index].value = "Fiber" ;
      } 
      else
      {
          arr_1_1_2[index].value = "Copper" ;    
      }
   }
    var arr_1_1_3 = getInputElementsByXid('1_1_3');
     for(var index=0;index<arr_1_1_3.length;index++){
      if(arr_1_1_3[index].value == "Link Up")
      {
         arr_1_1_3[index].value = "Up" ;
      }
      else if(arr_1_1_3[index].value == "Link Down")
      {
          arr_1_1_3[index].value = "Down" ;
      }
    }

    var arr_1_1_5 = getInputElementsByXid('1_1_5');
     for(var index=0;index<arr_1_1_5.length;index++){
      if(arr_1_1_5[index].value == "100MbpsHalfDuplex")
      {
         arr_1_1_5[index].value = "HALF-100-TX" ;
      }
      else if(arr_1_1_5[index].value == "100MbpsFullDuplex")
      {
          arr_1_1_5[index].value = "FULL-100-TX" ;
      }
      else if(arr_1_1_5[index].value == "10MbpsHalfDuplex")
      {
         arr_1_1_5[index].value = "HALF-10-T" ;
      }
      else if(arr_1_1_5[index].value == "10MbpsFullDuplex")
      {
        arr_1_1_5[index].value = "FULL-10-T" ;
      }
      else if(arr_1_1_5[index].value == "100MbpsFxFullDuplex")
      {
        arr_1_1_5[index].value = "FULL-100-FX" ;
      }
      else if(arr_1_1_5[index].value == "1000MbpsFullDuplex")
      {
        arr_1_1_5[index].value = "FULL-1000-SX" ;
      }
      else
      {
        arr_1_1_5[index].value = " ";
      }
    }
}


function port_summ_LoadEnd(arg)
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

