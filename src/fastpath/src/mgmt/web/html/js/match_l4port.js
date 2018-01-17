function getInputElementByXid(xid){



    var all_elements = document.getElementsByTagName('input') ;



    for(var index=0;index<all_elements.length;index++){

        var element = all_elements[index];

        if( element.getAttribute('xid') == null ||

            element.getAttribute('xid') != xid) continue;



        return element;

    }

}



function match_l4port_SubmitStart(submitFlag){

   

   var element_1_9_1 = getInputElementByXid('1_9_1');

   var element_1_9_2 = getInputElementByXid('1_9_2');

    if(element_1_9_2.value != element_1_9_1.nextSibling.value)

	{

      element_1_9_2.value = element_1_9_1.nextSibling.value;  

	  element_1_9_2.nextSibling.value = element_1_9_2.value;

	  

	}  

}





function match_l4port_SubmitEnd(submitFlag){

  

}



function match_l4port_LoadStart(arg){

   

}



function match_l4port_LoadEnd(arg){

 

}

