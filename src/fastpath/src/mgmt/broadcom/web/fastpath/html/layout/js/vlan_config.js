function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function vlan_config_SubmitStart(submitFlag){

}


function vlan_config_SubmitEnd(submitFlag){

}

function vlan_config_LoadStart(arg){

}

function vlan_config_LoadEnd(arg){
      var element_1_9_1 = getInputElementByXid('1_9_1');

       if(element_1_9_1 == null || element_1_9_1.nextSibling == null )
          return;

       element_1_9_1.nextSibling.rows="4";
       element_1_9_1.nextSibling.cols="56";

}

