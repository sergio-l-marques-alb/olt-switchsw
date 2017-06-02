function rip_redistribute_summary_SubmitStart(submitFlag){

}
function rip_redistribute_summary_SubmitEnd(submitFlag){
}
function rip_redistribute_summary_LoadStart(arg){
  
  var enum_options = new Object();

  enum_options.connected= "1";
  enum_options.statik = "2";
  enum_options.rip="3" ;
  enum_options.ospf="4";
  enum_options.bgp="5";

  var enum_vals= new Array();
  enum_vals[enum_options.connected]="Connected";
  enum_vals[enum_options.statik]="Static";
  enum_vals[enum_options.rip]="RIP";
  enum_vals[enum_options.ospf]="OSPF";
  enum_vals[enum_options.bgp]="BGP";

  var element_1_1_1_array = getAllInputElementsByXid('1_1_1');


  for(var index=0;index < element_1_1_1_array.length;index++){
      var element_1_1_1 = element_1_1_1_array[index];  
      var value = element_1_1_1.value;
      var source_string;
  
      switch(value){
        case enum_options.connected:
            source_string = enum_vals[enum_options.connected];
            break;
        case enum_options.statik:
            source_string = enum_vals[enum_options.statik];
            break;
        case enum_options.rip:
            source_string = enum_vals[enum_options.rip];
            break;
        case enum_options.ospf:
            source_string = enum_vals[enum_options.ospf];
            break;
        case enum_options.bgp:
            source_string = enum_vals[enum_options.bgp];
            break;
        default:
            source_string = null;
            break;
      }
  
      if(source_string != null)
      {
         element_1_1_1.value = source_string;
      }
 }
  // else
 //  {
 //     return;
 //  } 

}
function rip_redistribute_summary_LoadEnd(arg){
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

function getAllInputElementsByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;
    var return_array=new Array();

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if(!element.hasAttribute('xid') ||
            element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

            return_array[return_array.length] = element;
    }

    return return_array;
}



