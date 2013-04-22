function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function copy_serviceDetailedStats_SubmitStart(submitFlag){
 var element_1_6_1 = getInputElementByXid('1_6_1');
   var element_1_7_1 = getInputElementByXid('1_7_1');
   element_1_7_1.value = element_1_6_1.nextSibling.value;
   element_1_7_1.nextSibling.value = element_1_7_1.value;  
}


function copy_serviceDetailedStats_SubmitEnd(submitFlag){
/*
 var element_1_6_1 = getInputElementByXid('1_6_1');
   var element_1_7_1 = getInputElementByXid('1_7_1');
   element_1_7_1.value = element_1_6_1.nextSibling.value;
   element_1_7_1.nextSibling.value = element_1_7_1.value;
   */
 }

function copy_serviceDetailedStats_LoadStart(arg){
   var element_1_6_1 = getInputElementByXid('1_6_1');
   var element_1_7_1 = getInputElementByXid('1_7_1');
   element_1_7_1.value = element_1_6_1.value;
   
  
}

function copy_serviceDetailedStats_LoadEnd(arg){
/*
var element_1_6_1 = getInputElementByXid('1_6_1');
   var element_1_7_1 = getInputElementByXid('1_7_1');
   element_1_7_1.value = element_1_6_1.value;
   */
}
