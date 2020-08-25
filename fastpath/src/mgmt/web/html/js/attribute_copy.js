function getInputElementByXid(xid){



    var all_elements = document.getElementsByTagName('input') ;



    for(var index=0;index<all_elements.length;index++){

        var element = all_elements[index];

        if( element.getAttribute('xid') == null ||

            element.getAttribute('xid') != xid) continue;



        return element;

    }

}



function attribute_copy_SubmitStart(submitFlag){

   

   var element_1_6_1 = getInputElementByXid('1_6_1');

   var element_1_6_2 = getInputElementByXid('1_6_2');

   var element_1_6_3 = getInputElementByXid('1_6_3');

   var element_1_6_4 = getInputElementByXid('1_6_4');

   var element_1_6_5 = getInputElementByXid('1_6_5');

   var element_1_6_6 = getInputElementByXid('1_6_6');

   var element_1_6_7 = getInputElementByXid('1_6_7');

   

   var element = getInputElementByXid('1_1_3');

   

    if(element_1_6_1.disabled == false) {

	      element.value = element_1_6_1.value;

	}else if(element_1_6_2.disabled == false){

	

    element.value = element_1_6_2.value;	

	}else if(element_1_6_3.disabled == false){

	

    element.value = element_1_6_3.value;	

	}else if(element_1_6_4.disabled == false){

	

    element.value = element_1_6_4.value;	

	}else if(element_1_6_5.disabled == false){

	

    element.value = element_1_6_5.value;	

	}else if(element_1_6_6.disabled == false){

	

    element.value = element_1_6_6.value;	

	}else if(element_1_6_7.disabled == false){

	

    element.value = element_1_6_7.value;	

	} 

	element.nextSibling.value = element.value;

	

}





function attribute_copy_SubmitEnd(submitFlag){

  

}



function attribute_copy_LoadStart(arg){

   

}



function attribute_copy_LoadEnd(arg){

 

}
