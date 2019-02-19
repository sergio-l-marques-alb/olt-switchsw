function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function ping_ipv6_SubmitStart(submitFlag){

}


function ping_ipv6_SubmitEnd(submitFlag){

}

function ping_ipv6_LoadStart(arg){

}

function ping_ipv6_LoadEnd(arg){
      var element_1_5_1 = getInputElementByXid('1_5_1');

       if(element_1_5_1 == null || element_1_5_1.nextSibling == null )
          return;

       element_1_5_1.nextSibling.rows="7";
       element_1_5_1.nextSibling.cols="56";
       element_1_5_1.nextSibling.readOnly = true;
       element_1_5_1.nextSibling.overflow = scroll;
       element_1_5_1.nextSibling.disabled = false;
}

