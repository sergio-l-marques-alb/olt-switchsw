
function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function dual_image_status_SubmitStart(submitFlag){

}


function dual_image_status_SubmitEnd(submitFlag){

}

function dual_image_status_LoadStart(arg){

}

function dual_image_status_LoadEnd(arg){
      var element_1_2_1 = getInputElementByXid('1_2_1');
      var element_1_4_1 = getInputElementByXid('1_4_1');

       if(element_1_2_1 == null || element_1_2_1.nextSibling == null )
          return;

       element_1_2_1.nextSibling.rows="3";
       element_1_2_1.nextSibling.cols="64";
       element_1_2_1.nextSibling.readOnly = true;
       element_1_2_1.nextSibling.overflow = scroll;
       element_1_2_1.nextSibling.disabled = false;


       if(element_1_4_1 == null || element_1_4_1.nextSibling == null )
          return;

       element_1_4_1.nextSibling.rows="3";
       element_1_4_1.nextSibling.cols="64";
       element_1_4_1.nextSibling.readOnly = true;
       element_1_4_1.nextSibling.overflow = scroll;
       element_1_4_1.nextSibling.disabled = false;

}

