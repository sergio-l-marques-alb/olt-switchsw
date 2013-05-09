function port_SubmitStart(submitFlag)
{
}
function port_SubmitEnd(submitFlag)
{
}

function port_LoadStart(arg)
{
  var arr_1_3_1 = getInputElementByXid('1_3_1');
  
      if(arr_1_3_1.value == "L7_PHY_CAP_PORTSPEED_SFP" || arr_1_3_1.value == "L7_PHY_CAP_PORTSPEED_SFP_DETECT")
      {
         arr_1_3_1.value = "Fiber" ;
      } 
      else
      {
          arr_1_3_1.value = "Copper" ;    
      }
   
  var arr_1_4_1 = getInputElementByXid('1_4_1');
      if(arr_1_4_1.value == "Link Up")
      {
         arr_1_4_1.value = "Up" ;
      }
      else if(arr_1_4_1.value == "Link Down")
      {
          arr_1_4_1.value = "Down" ;
      }
    
}


function port_LoadEnd(arg)
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
