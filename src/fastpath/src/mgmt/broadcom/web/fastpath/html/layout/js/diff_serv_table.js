function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input');

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function diff_serv_table_SubmitStart(submitFlag){

}


function diff_serv_table_SubmitEnd(submitFlag){

}

function diff_serv_table_LoadStart(arg){
      var element_3_8_1 = getInputElementByXid('3_8_1');
      var element_3_8_2 = getInputElementByXid('3_8_2');
      var element_3_8_3 = getInputElementByXid('3_8_3');
      var element_3_8_4 = getInputElementByXid('3_8_4');
      var element_3_8_5 = getInputElementByXid('3_8_5');
      var element_3_8_6 = getInputElementByXid('3_8_6');
      var element_3_8_7 = getInputElementByXid('3_8_7');
      var element_3_8_8 = getInputElementByXid('3_8_8');
      var element_3_8_9 = getInputElementByXid('3_8_9');
      var element_3_8_10 = getInputElementByXid('3_8_10');
      var element_3_8_11 = getInputElementByXid('3_8_11');
      var element_3_8_12 = getInputElementByXid('3_8_12');

      var element_3_2_2 = getInputElementByXid('3_2_2');
      var element_3_3_2 = getInputElementByXid('3_3_2');
      var element_3_4_2 = getInputElementByXid('3_4_2');
      var element_3_5_2 = getInputElementByXid('3_5_2');
      var element_3_6_2 = getInputElementByXid('3_6_2');
      var element_3_7_2 = getInputElementByXid('3_7_2');

      element_3_2_2.value=element_3_8_11.value+"/"+element_3_8_12.value;
      element_3_3_2.value=element_3_8_1.value+"/"+element_3_8_2.value;
      element_3_4_2.value=element_3_8_3.value+"/"+element_3_8_4.value;
      element_3_5_2.value=element_3_8_5.value+"/"+element_3_8_6.value;
      element_3_6_2.value=element_3_8_7.value+"/"+element_3_8_8.value;
      element_3_7_2.value=element_3_8_9.value+"/"+element_3_8_10.value;

}

function diff_serv_table_LoadEnd(arg){

}

