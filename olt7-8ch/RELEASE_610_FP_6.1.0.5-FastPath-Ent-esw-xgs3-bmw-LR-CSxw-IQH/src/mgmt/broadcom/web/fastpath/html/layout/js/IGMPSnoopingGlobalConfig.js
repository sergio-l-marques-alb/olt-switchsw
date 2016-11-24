function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function IGMPSnoopingGlobalConfig_SubmitStart(submitFlag){

}


function IGMPSnoopingGlobalConfig_SubmitEnd(submitFlag){

}

function IGMPSnoopingGlobalConfig_LoadStart(arg){

}

function IGMPSnoopingGlobalConfig_LoadEnd(arg){
      var element_1_5_1 = getInputElementByXid('1_5_1');
      var element_1_7_1 = getInputElementByXid('1_7_1');

       if(element_1_5_1 == null || element_1_5_1.nextSibling == null )
          return;

       element_1_5_1.nextSibling.rows="7";
       element_1_5_1.nextSibling.cols="17";
       element_1_5_1.nextSibling.readOnly = true;
       element_1_5_1.nextSibling.overflow = scroll;
       element_1_5_1.nextSibling.disabled = false;

       if(element_1_7_1 == null || element_1_7_1.nextSibling == null )
          return;

       element_1_7_1.nextSibling.rows="7";
       element_1_7_1.nextSibling.cols="13";
       element_1_5_1.nextSibling.readOnly = true;
       element_1_7_1.nextSibling.overflow = scroll;
       element_1_7_1.nextSibling.disabled = false;
}

