function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function InventoryInfo_SubmitStart(submitFlag){

}


function InventoryInfo_SubmitEnd(submitFlag){

}

function InventoryInfo_LoadStart(arg){

}

function InventoryInfo_LoadEnd(arg){
      var element_1_14_1 = getInputElementByXid('1_14_1');

       if(element_1_14_1 == null || element_1_14_1.nextSibling == null )
          return;

       element_1_14_1.nextSibling.rows="7";
       element_1_14_1.nextSibling.cols="30";

}

