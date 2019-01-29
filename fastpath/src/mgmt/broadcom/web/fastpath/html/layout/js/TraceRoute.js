xuiIncludeJSFile("/scripts/xui_oem.js");

function TraceRoute_SubmitStart(submitFlag){

}


function TraceRoute_SubmitEnd(submitFlag){

}

function TraceRoute_LoadStart(arg){

}

function TraceRoute_LoadEnd(arg){
      var element_1_9_1 = getInputElementByXid('1_9_1');

       if(element_1_9_1 == null || element_1_9_1.nextSibling == null )
          return;

       element_1_9_1.nextSibling.rows="5";
       element_1_9_1.nextSibling.cols="70";
       element_1_9_1.nextSibling.disabled=false;
       element_1_9_1.nextSibling.readOnly=true;

}

