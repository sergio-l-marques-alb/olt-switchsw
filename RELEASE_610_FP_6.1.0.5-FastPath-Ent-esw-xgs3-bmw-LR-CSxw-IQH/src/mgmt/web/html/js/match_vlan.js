function getInputElementByXid(xid){



    var all_elements = document.getElementsByTagName('input') ;



    for(var index=0;index<all_elements.length;index++){

        var element = all_elements[index];

        if( element.getAttribute('xid') == null ||

            element.getAttribute('xid') != xid) continue;



        return element;

    }

}



function match_vlan_SubmitStart(submitFlag){

   

   var element_1_6_1 = getInputElementByXid('1_6_1');

   var element_1_6_2 = getInputElementByXid('1_6_2');

   

    element_1_6_2.value = element_1_6_1.nextSibling.value;

   

}





function match_vlan_SubmitEnd(submitFlag){

  

}



function match_vlan_LoadStart(arg){

   

}



function match_vlan_LoadEnd(arg){

 

}
