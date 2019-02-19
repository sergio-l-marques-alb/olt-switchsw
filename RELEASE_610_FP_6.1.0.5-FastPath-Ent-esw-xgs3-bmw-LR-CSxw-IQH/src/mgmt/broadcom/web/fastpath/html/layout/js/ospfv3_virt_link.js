function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if(element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function ospfv3_virt_link_SubmitStart(submitFlag){
   var element_1_1_1 = getInputElementByXid('1_1_1'); 
   var element_1_3_1 = getInputElementByXid('1_3_1');
   var element_1_3_2 = getInputElementByXid('1_3_2');

   var index_1_1_1 = element_1_1_1.nextSibling.selectedIndex;
   var index_1_3_1 = element_1_3_1.nextSibling.selectedIndex;

   if(element_1_1_1.nextSibling.options[index_1_1_1].text == "Create New Virtual Link")
   {
     element_1_3_2.value = element_1_3_1.nextSibling.options[index_1_3_1].text;
   }
}


function ospfv3_virt_link_SubmitEnd(submitFlag){
  
}

function ospfv3_virt_link_LoadStart(arg){
   
}

function ospfv3_virt_link_LoadEnd(arg){
 
}
