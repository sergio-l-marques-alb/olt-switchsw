xuiIncludeJSFile("/scripts/globalStyleJS.js");


function getAllInputElementsByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;
    var resultArray  = new Array();

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if(element.getAttribute('xid') == null ||
           element.getAttribute('xid') != xid) continue;

        resultArray.push(element);
    }
    
    return resultArray;
}
 /* xuiGetRepeatElement()
     parameters :
                table Id
                key element Id
                Id of the element required
                key value in the table to match a particular row

    return     : element in the row with specified key value
  */

function PIM6_SSM_Configuration_Add_LoadEnd(arg){
     globalStyleJS_LoadEnd();
}
function PIM6_SSM_Configuration_Add_LoadStart(arg){

   var inputs_2_7_2 = getAllInputElementsByXid('2_7_2');
   var inputs_2_7_3 = getAllInputElementsByXid('2_7_3');

   
   if ((inputs_2_7_2==null) || (inputs_2_7_3==null))
   {
     globalStyleJS_LoadStart();
     return;
     
   }

   for(var index=0;index<inputs_2_7_2.length;index++)
   {
     if (((inputs_2_7_2[index].value=="FF30::") || (inputs_2_7_2[index].value=="ff30::")) &&
         (inputs_2_7_3[index].value=="32") )
     {
       inputs_2_7_2[index].value = "FF3x::";  
     }
   }
   globalStyleJS_LoadStart();
}

function PIM6_SSM_Configuration_Add_SubmitStart(submitFlag){
   globalStyleJS_SubmitStart();
}

function PIM6_SSM_Configuration_Add_SubmitEnd(submitFlag){
   var inputs_2_7_2 = getAllInputElementsByXid('2_7_2');
   
   if (inputs_2_7_2==null) 
   {
     globalStyleJS_SubmitEnd();
     return;
   }

   for(var index=0;index<inputs_2_7_2.length;index++)
   {
     if (inputs_2_7_2[index].value=="FF3x::") 
     {
       inputs_2_7_2[index].value = "FF30::";  
     }
   }
   globalStyleJS_SubmitEnd();
}

