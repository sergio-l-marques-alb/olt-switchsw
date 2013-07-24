function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function acltable_SubmitStart(submitFlag){

}


function acltable_SubmitEnd(submitFlag){

}

function acltable_LoadStart(arg){
      var element_2_1_1 = getInputElementByXid('2_1_1');
      var element_2_1_2 = getInputElementByXid('2_1_2');
      var element_2_3_2 = getInputElementByXid('2_3_2');

      element_2_3_2.value=element_2_1_1.value+"/"+element_2_1_2.value;

}

function acltable_LoadEnd(arg){

}

