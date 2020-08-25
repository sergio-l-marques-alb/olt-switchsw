function OSPFv3InterfaceConfig_LoadStart(arg){
}
function OSPFv3InterfaceConfig_LoadEnd(arg){

/* 
Variables Declaration  For READ-WRITE user 
*/
   var inputs_1_20_1 = GetElementsByXid('1_20_1','INPUT');
   var inputs_1_20_2 = GetElementsByXid('1_20_2','INPUT');
   var inputs_1_20_3 = GetElementsByXid('1_20_3','INPUT');

   var inputs_1_2_1 = GetElementsByXid('1_2_1','INPUT');

   var ipv6AddrRW="";

/* Variables Declaration For Read-Only User
*/
   var inputs_2_20_1 = GetElementsByXid('2_20_1','INPUT');
   var inputs_2_20_2 = GetElementsByXid('2_20_2','INPUT');
   var inputs_2_20_3 = GetElementsByXid('2_20_3','INPUT');

   var inputs_2_2_1 = GetElementsByXid('2_2_1','INPUT');

   var ipv6AddrRO="";

   if ((inputs_1_20_1 != null) && (inputs_1_20_2 != null) && (inputs_1_2_1 != null))
   {

     for(var index=0;index<inputs_1_20_1.length;index++)
     {
       if (index <inputs_1_20_2.length)
       {
          var state="";
          if (inputs_1_20_3 != null)
          {
            if (index < inputs_1_20_3.length)
            {
              if (inputs_1_20_3[index].value != "Active")
              {
                state = inputs_1_20_3[index].value;
              }
             }
           }
           ipv6AddrRW+=inputs_1_20_1[index].value+"/"+inputs_1_20_2[index].value+state+"<br/>";
       }
     }
     if (inputs_1_20_2.length != 0 )
     {
       inputs_1_2_1[0].parentNode.innerHTML = ipv6AddrRW;
       return;
     }
   }
   if ((inputs_2_20_1 != null) && (inputs_2_20_2 != null) && (inputs_2_2_1 != null))
   {
     for(var index=0;index<inputs_2_20_1.length;index++)
     {
       if (index <inputs_2_20_2.length)
       {
          var state="";
          if (inputs_2_20_3 != null)
          {
            if (index < inputs_2_20_3.length)
            {
              if (inputs_2_20_3[index].value != "Active")
              {
                state = inputs_2_20_3[index].value;
              }
             }
           }
           ipv6AddrRO+=inputs_2_20_1[index].value+"/"+inputs_2_20_2[index].value+state+"<br/>";
       }
     }
     if (inputs_2_20_1.length != 0)
     {
       inputs_2_2_1[0].parentNode.innerHTML = ipv6AddrRO;
       return;
     }
   }
}

function OSPFv3InterfaceConfig_SubmitStart(submitFlag){
}

function OSPFv3InterfaceConfig_SubmitEnd(submitFlag){
}



