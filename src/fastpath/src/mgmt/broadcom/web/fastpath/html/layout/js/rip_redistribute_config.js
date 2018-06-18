function rip_redistribute_config_SubmitStart(submitFlag){
  var enum_options = new Object();
  enum_options.create = "Create" ;
  enum_options.statik = "Static";
  enum_options.connected= "Connected";
  enum_options.rip="RIP" ; 
  enum_options.bgp="BGP";
  enum_options.ospf="OSPF";
  
  var enum_vals= new Array();
  enum_vals[enum_options.create]=0;
  enum_vals[enum_options.connected]=1;
  enum_vals[enum_options.statik]=2;
  enum_vals[enum_options.rip]=3;
  enum_vals[enum_options.bgp]=5;
  enum_vals[enum_options.ospf]=4;
  

  var element_1_2_1 = getInputElementByXid('1_2_1');

   if(element_1_2_1 == null || element_1_2_1.nextSibling == null) {
     return;
   }
  
   var options = element_1_2_1.nextSibling.options;
   var value=options[element_1_2_1.nextSibling.selectedIndex].value;

   switch(value){
       case enum_options.create:
             var element_1_3_1 = getInputElementByXid('1_3_1');

             if(element_1_3_1 != null || element_1_3_1.nextSibling != null && 
                element_1_3_1.nextSibling.options != null ) {
                key_val = element_1_3_1.nextSibling.options[element_1_3_1.nextSibling.selectedIndex].value
             }
             break;
       default:
             key_val = value;
   } 
   switch(key_val){
       case enum_options.connected:
            key_val = enum_vals[enum_options.connected];
            break;
       case enum_options.statik:
            key_val = enum_vals[enum_options.statik];
            break;
       case enum_options.rip:
            key_val = enum_vals[enum_options.rip];
            break;
       case enum_options.bgp:
            key_val = enum_vals[enum_options.bgp];
            break;
       case enum_options.ospf:
            key_val = enum_vals[enum_options.ospf];
            break;
       default:
            key_val = null;
   }

   if(key_val != null){
       var element_1_1_1 = getInputElementByXid('1_1_1');

             if(element_1_1_1 != null){
                element_1_1_1.value = key_val;
             } 
   }
  
}
function rip_redistribute_config_SubmitEnd(submitFlag){
}
function rip_redistribute_config_LoadStart(arg){

}
function rip_redistribute_config_LoadEnd(arg){
}
function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if(!element.hasAttribute('xid') ||
            element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }


}




