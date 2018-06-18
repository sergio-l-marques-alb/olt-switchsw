function OSPFv3InterfaceStats_LoadStart(arg){
}
function OSPFv3InterfaceStats_LoadEnd(arg){

   var inputs_1_35_1 = GetElementsByXid('1_35_1','INPUT');
   var inputs_1_35_2 = GetElementsByXid('1_35_2','INPUT');
   var inputs_1_35_3 = GetElementsByXid('1_35_3','INPUT');

   var inputs_1_7_1 = GetElementsByXid('1_7_1','INPUT');

   var ipv6Addr="";

   if ((inputs_1_35_1 == null) || (inputs_1_35_2==null) || (inputs_1_7_1==null))
   {
     return;

   }

   for(var index=0;index<inputs_1_35_1.length;index++)
   {
      if (index < inputs_1_35_2.length)
      {
        var state="";
         if (inputs_1_35_3 != null)
         {
           if (index < inputs_1_35_3.length)
           {
             if (inputs_1_35_3[index].value != "Active")
             {
               state = inputs_1_35_3[index].value;
             }
           }
         }
         ipv6Addr+=inputs_1_35_1[index].value+"/"+inputs_1_35_2[index].value+state+"<br/>";
       }
   }
   inputs_1_7_1[0].parentNode.innerHTML = ipv6Addr;

}

function OSPFv3InterfaceStats_SubmitStart(submitFlag){
}

function OSPFv3InterfaceStats_SubmitEnd(submitFlag){
}



