function diffserv_policesimple_SubmitStart(submitFlag)
{
}


function diffserv_policesimple_SubmitEnd(submitFlag)
{
}

function diffserv_policesimple_LoadStart(arg)
{
//Decide the lease time 	
   var arg_1_13_1 = getInputElementByXid('1_13_1');
   var arg_1_24_1 = getInputElementByXid('1_24_1');
   var arg_1_26_1 =  getInputElementByXid('1_26_1'); 
   var arg_1_15_1 =  getInputElementByXid('1_15_1');
   var arg_1_16_3 =  getInputElementByXid('1_16_3');
   var arg_1_28_1 =  getInputElementByXid('1_28_1');
   if(arg_1_13_1.value == "DSCP")
   {
     if(arg_1_24_1.value == "10")
     {
        arg_1_26_1.value = "af11";
     }
     else if(arg_1_24_1.value == "12")
     {
        arg_1_26_1.value = "af12";
     }
     else if(arg_1_24_1.value == "14")
     {
        arg_1_26_1.value = "af13";
     }
     else if(arg_1_24_1.value == "18")
     {
        arg_1_26_1.value = "af21";
     }
     else if(arg_1_24_1.value == "20")
     {
        arg_1_26_1.value = "af22";
     }
     else if(arg_1_24_1.value == "22")
     {
        arg_1_26_1.value = "af23";
     }
     else if(arg_1_24_1.value == "26")
     {
        arg_1_26_1.value = "af31";
     }
     else if(arg_1_24_1.value == "28")
     {
        arg_1_26_1.value = "af32";
     }
     else if(arg_1_24_1.value == "30")
     {
        arg_1_26_1.value = "af33";
     }
     else if(arg_1_24_1.value == "34")
     {
        arg_1_26_1.value = "af41";
     }
     else if(arg_1_24_1.value == "36")
     {
        arg_1_26_1.value = "af42";
     }
     else if(arg_1_24_1.value == "38")
     {
        arg_1_26_1.value = "af43";
     }
     else if(arg_1_24_1.value == "0")
     {
        arg_1_26_1.value = "BE";
     }
     else if(arg_1_24_1.value == "0")
     {
        arg_1_26_1.value = "CS0";
     }
     else if(arg_1_24_1.value == "8")
     {
        arg_1_26_1.value = "CS1";
     }
     else if(arg_1_24_1.value == "16")
     {
        arg_1_26_1.value = "CS2";
     }
     else if(arg_1_24_1.value == "24")
     {
        arg_1_26_1.value = "CS3";
     }
     else if(arg_1_24_1.value == "32")
     {
        arg_1_26_1.value = "CS4";
     }
     else if(arg_1_24_1.value == "40")
     {
        arg_1_26_1.value = "CS5";
     }
     else if(arg_1_24_1.value == "48")
     {
        arg_1_26_1.value = "CS6";
     }
     else if(arg_1_24_1.value == "56")
     {
        arg_1_26_1.value = "CS7";
     }
     else if(arg_1_24_1.value == "46")
     {
        arg_1_26_1.value = "EF";
     }
   }
   
   if(arg_1_15_1.value == "DSCP")
   {
     if(arg_1_16_3.value == "10")
     {
        arg_1_28_1.value = "af11";
     }
     else if(arg_1_16_3.value == "12")
     {
        arg_1_28_1.value = "af12";
     }
     else if(arg_1_16_3.value == "14")
     {
        arg_1_28_1.value = "af13";
     }
     else if(arg_1_16_3.value == "18")
     {
        arg_1_28_1.value = "af21";
     }
     else if(arg_1_16_3.value == "20")
     {
        arg_1_28_1.value = "af22";
     }
     else if(arg_1_16_3.value == "22")
     {
        arg_1_28_1.value = "af23";
     }
     else if(arg_1_16_3.value == "26")
     {
        arg_1_28_1.value = "af31";
     }
     else if(arg_1_16_3.value == "28")
     {
        arg_1_28_1.value = "af32";
     }
     else if(arg_1_16_3.value == "30")
     {
        arg_1_28_1.value = "af33";
     }
     else if(arg_1_16_3.value == "34")
     {
        arg_1_28_1.value = "af41";
     }
     else if(arg_1_16_3.value == "36")
     {
        arg_1_28_1.value = "af42";
     }
     else if(arg_1_16_3.value == "38")
     {
        arg_1_28_1.value = "af43";
     }
     else if(arg_1_16_3.value == "0")
     {
        arg_1_28_1.value = "BE";
     }
     else if(arg_1_16_3.value == "0")
     {
        arg_1_28_1.value = "CS0";
     }
     else if(arg_1_16_3.value == "8")
     {
        arg_1_28_1.value = "CS1";
     }
     else if(arg_1_16_3.value == "16")
     {
        arg_1_28_1.value = "CS2";
     }
     else if(arg_1_16_3.value == "24")
     {
        arg_1_28_1.value = "CS3";
     }
     else if(arg_1_16_3.value == "32")
     {
        arg_1_28_1.value = "CS4";
     }
     else if(arg_1_16_3.value == "40")
     {
        arg_1_28_1.value = "CS5";
     }
     else if(arg_1_16_3.value == "48")
     {
        arg_1_28_1.value = "CS6";
     }
     else if(arg_1_16_3.value == "56")
     {
        arg_1_28_1.value = "CS7";
     }
     else if(arg_1_16_3.value == "46")
     {
        arg_1_28_1.value = "EF";
     }
   }
}

function diffserv_policesimple_LoadEnd(arg)
{
}

function getInputElementByXid(xid)
{
    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++)
    {
        var element = all_elements[index];
        if(element.getAttribute('xid') == null ||
           element.getAttribute('xid') != xid) continue;

        return element;
    }


}

