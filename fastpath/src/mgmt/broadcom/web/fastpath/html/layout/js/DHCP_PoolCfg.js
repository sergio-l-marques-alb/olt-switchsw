function DHCP_PoolCfg_SubmitStart(submitFlag)
{
//alert("submit start...");
    var temp1;
    var leaseTimeSelect =  getInputElementByXid('1_15_1');
    var leaseTime = getInputElementByXid('1_19_1');

//Assigning Lease Time object with the values of days,hours and minutes
   var element = getInputElementByXid('1_16_1');
   if(element == null || element.value == null) return;
   var days = element.nextSibling.value; 
   var dayscurr = element.value;

   element = getInputElementByXid('1_17_1');
   if(element == null || element.value == null) return;
   var hours = element.nextSibling.value;
   var hourscurr = element.value;
   element = getInputElementByXid('1_18_1');	
   if(element == null || element.value == null) return;
   var minutes = element.nextSibling.value;
   var minutescurr = element.value;
   if((parseInt(days) < 0)||(parseInt(days) > 59)) 
   {
     days = dayscurr;
     alert("Error!Days should be in the range of 0-59");
   }
   if((parseInt(hours)<0)||(parseInt(hours)>23))
   {
      hours = hourscurr;
      alert("Error!Hours should be in the range of 0-23");
   }
   if((parseInt(minutes)<0)||(parseInt(minutes)>59))
   {
      minutes = minutescurr; 
      alert("Error!Minutes should be in the range of 0-59");
   }
   
   if(leaseTimeSelect.nextSibling.options[1].selected == true)
   {	
    temp1 = (parseInt(days) * 24 * 60) + (parseInt(hours) * 60) + parseInt(minutes);
   }
   else
   {
     temp1 = "4294967295";	   
   }	  
   leaseTime.value = temp1;    

//Assign Def Router Address object
   var DefRtrbuffer;

   var element_1_21_1  =  getInputElementByXid('1_21_1');
   if(element_1_21_1.nextSibling.value == null || element_1_21_1.nextSibling.value == "" || element_1_21_1.nextSibling.value == " ")
     element_1_21_1.nextSibling.value = "0.0.0.0";
     DefRtrbuffer = element_1_21_1.nextSibling.value+",";

   var element_1_22_1  =  getInputElementByXid('1_22_1');
   if(element_1_22_1.nextSibling.value == null || element_1_22_1.nextSibling.value == "" || element_1_22_1.nextSibling.value == " ")
      element_1_22_1.nextSibling.value = "0.0.0.0";
     DefRtrbuffer += element_1_22_1.nextSibling.value+",";

   var element_1_23_1  =  getInputElementByXid('1_23_1');
   if(element_1_23_1.nextSibling.value == null || element_1_23_1.nextSibling.value == "" || element_1_23_1.nextSibling.value == " ")
      element_1_23_1.nextSibling.value = "0.0.0.0";
     DefRtrbuffer += element_1_23_1.nextSibling.value+",";

   var element_1_24_1  =  getInputElementByXid('1_24_1');
   if(element_1_24_1.nextSibling.value == null || element_1_24_1.nextSibling.value == "" || element_1_24_1.nextSibling.value == " ")
      element_1_24_1.nextSibling.value = "0.0.0.0";
     DefRtrbuffer += element_1_24_1.nextSibling.value+","; 

   var element_1_25_1  =  getInputElementByXid('1_25_1');
   if(element_1_25_1.nextSibling.value == null || element_1_25_1.nextSibling.value == "" || element_1_25_1.nextSibling.value == " ")
      element_1_25_1.nextSibling.value = "0.0.0.0";
     DefRtrbuffer += element_1_25_1.nextSibling.value+",";

   var element_1_26_1  =  getInputElementByXid('1_26_1');
   if(element_1_26_1.nextSibling.value == null || element_1_26_1.nextSibling.value == "" || element_1_26_1.nextSibling.value == " ")
      element_1_26_1.nextSibling.value = "0.0.0.0";
     DefRtrbuffer += element_1_26_1.nextSibling.value+",";

   var element_1_27_1  =  getInputElementByXid('1_27_1');
   if(element_1_27_1.nextSibling.value == null || element_1_27_1.nextSibling.value == "" || element_1_27_1.nextSibling.value == " ")
      element_1_27_1.nextSibling.value = "0.0.0.0";
    DefRtrbuffer += element_1_27_1.nextSibling.value+",";

   var element_1_28_1  =  getInputElementByXid('1_28_1');
   if(element_1_28_1.nextSibling.value == null || element_1_28_1.nextSibling.value == "" || element_1_28_1.nextSibling.value == " ")
      element_1_28_1.nextSibling.value = "0.0.0.0";
    DefRtrbuffer += element_1_28_1.nextSibling.value;

   var element_1_29_1 = getInputElementByXid('1_29_1');
   element_1_29_1.value = DefRtrbuffer; 

//Populate the DNS Server Object

  var Dnsbuffer;

  var element_1_31_1  =  getInputElementByXid('1_31_1');
   if(element_1_31_1.nextSibling.value == null || element_1_31_1.nextSibling.value == "" || element_1_31_1.nextSibling.value == " ")
     element_1_31_1.nextSibling.value = "0.0.0.0";
     Dnsbuffer = element_1_31_1.nextSibling.value+",";

   var element_1_32_1  =  getInputElementByXid('1_32_1');
   if(element_1_32_1.nextSibling.value == null || element_1_32_1.nextSibling.value == "" || element_1_32_1.nextSibling.value == " ")
      element_1_32_1.nextSibling.value = "0.0.0.0";
     Dnsbuffer += element_1_32_1.nextSibling.value+",";

   var element_1_33_1  =  getInputElementByXid('1_33_1');
   if(element_1_33_1.nextSibling.value == null || element_1_33_1.nextSibling.value == "" || element_1_33_1.nextSibling.value == " ")
      element_1_33_1.nextSibling.value = "0.0.0.0";
     Dnsbuffer += element_1_33_1.nextSibling.value+",";

   var element_1_34_1  =  getInputElementByXid('1_34_1');
   if(element_1_34_1.nextSibling.value == null || element_1_34_1.nextSibling.value == "" || element_1_34_1.nextSibling.value == " ")
      element_1_34_1.nextSibling.value = "0.0.0.0";
     Dnsbuffer += element_1_34_1.nextSibling.value+","; 

   var element_1_35_1  =  getInputElementByXid('1_35_1');
   if(element_1_35_1.nextSibling.value == null || element_1_35_1.nextSibling.value == "" || element_1_35_1.nextSibling.value == " ")
      element_1_35_1.nextSibling.value = "0.0.0.0";
     Dnsbuffer += element_1_35_1.nextSibling.value+",";

   var element_1_36_1  =  getInputElementByXid('1_36_1');
   if(element_1_36_1.nextSibling.value == null || element_1_36_1.nextSibling.value == "" || element_1_36_1.nextSibling.value == " ")
      element_1_36_1.nextSibling.value = "0.0.0.0";
     Dnsbuffer += element_1_36_1.nextSibling.value+",";

   var element_1_37_1  =  getInputElementByXid('1_37_1');
   if(element_1_37_1.nextSibling.value == null || element_1_37_1.nextSibling.value == "" || element_1_37_1.nextSibling.value == " ")
      element_1_37_1.nextSibling.value = "0.0.0.0";
    Dnsbuffer += element_1_37_1.nextSibling.value+",";

   var element_1_38_1  =  getInputElementByXid('1_38_1');
   if(element_1_38_1.nextSibling.value == null || element_1_38_1.nextSibling.value == "" || element_1_38_1.nextSibling.value == " ")
      element_1_38_1.nextSibling.value = "0.0.0.0";
    Dnsbuffer += element_1_38_1.nextSibling.value;

   var element_1_39_1 = getInputElementByXid('1_39_1');
   element_1_39_1.value = Dnsbuffer; 

//Populate the NETBIOSServer Object
 
  var Netbuffer;

  var element_1_41_1  =  getInputElementByXid('1_41_1');
   if(element_1_41_1.nextSibling.value == null || element_1_41_1.nextSibling.value == "" || element_1_41_1.nextSibling.value == " ")
     element_1_41_1.nextSibling.value = "0.0.0.0";
     Netbuffer = element_1_41_1.nextSibling.value+",";

   var element_1_42_1  =  getInputElementByXid('1_42_1');
   if(element_1_42_1.nextSibling.value == null || element_1_42_1.nextSibling.value == "" || element_1_42_1.nextSibling.value == " ")
      element_1_42_1.nextSibling.value = "0.0.0.0";
     Netbuffer += element_1_42_1.nextSibling.value+",";

   var element_1_43_1  =  getInputElementByXid('1_43_1');
   if(element_1_43_1.nextSibling.value == null || element_1_43_1.nextSibling.value == "" || element_1_43_1.nextSibling.value == " ")
      element_1_43_1.nextSibling.value = "0.0.0.0";
     Netbuffer += element_1_43_1.nextSibling.value+",";

   var element_1_44_1  =  getInputElementByXid('1_44_1');
   if(element_1_44_1.nextSibling.value == null || element_1_44_1.nextSibling.value == "" || element_1_44_1.nextSibling.value == " ")
      element_1_44_1.nextSibling.value = "0.0.0.0";
     Netbuffer += element_1_44_1.nextSibling.value+","; 

   var element_1_45_1  =  getInputElementByXid('1_45_1');
   if(element_1_45_1.nextSibling.value == null || element_1_45_1.nextSibling.value == "" || element_1_45_1.nextSibling.value == " ")
      element_1_45_1.nextSibling.value = "0.0.0.0";
     Netbuffer += element_1_45_1.nextSibling.value+",";

   var element_1_46_1  =  getInputElementByXid('1_46_1');
   if(element_1_46_1.nextSibling.value == null || element_1_46_1.nextSibling.value == "" || element_1_46_1.nextSibling.value == " ")
      element_1_46_1.nextSibling.value = "0.0.0.0";
     Netbuffer += element_1_46_1.nextSibling.value+",";

   var element_1_47_1  =  getInputElementByXid('1_47_1');
   if(element_1_47_1.nextSibling.value == null || element_1_47_1.nextSibling.value == "" || element_1_47_1.nextSibling.value == " ")
      element_1_47_1.nextSibling.value = "0.0.0.0";
    Netbuffer += element_1_47_1.nextSibling.value+",";

   var element_1_48_1  =  getInputElementByXid('1_48_1');
   if(element_1_48_1.nextSibling.value == null || element_1_48_1.nextSibling.value == "" || element_1_48_1.nextSibling.value == " ")
      element_1_48_1.nextSibling.value = "0.0.0.0";
    Netbuffer += element_1_48_1.nextSibling.value;

   var element_1_49_1 = getInputElementByXid('1_49_1');
   element_1_49_1.value = Netbuffer; 

//Populate the IPAddressData Object
   var ipAddrbuffer="";

   var element_1_60_1  =  getInputElementByXid('1_60_1');
   if(element_1_60_1 == null)
     return;
   
    if(element_1_60_1.nextSibling.value != null && element_1_60_1.nextSibling.value != "" && element_1_60_1.nextSibling.value != " " && element_1_60_1.nextSibling.value != "0.0.0.0")
    { 
     ipAddrbuffer = element_1_60_1.nextSibling.value+",";
    }

   var element_1_61_1  =  getInputElementByXid('1_61_1');
   if(element_1_61_1 == null) 
     return;
   if(element_1_61_1.nextSibling.value != "0.0.0.0" && element_1_61_1.nextSibling.value != null && element_1_61_1.nextSibling.value !="" && element_1_61_1.nextSibling.value != " ")
   {
     ipAddrbuffer += element_1_61_1.nextSibling.value+",";
   }

   var element_1_62_1  =  getInputElementByXid('1_62_1');
   if(element_1_62_1 == null)
     return;
   if(element_1_62_1.nextSibling.value != "0.0.0.0" && element_1_62_1.nextSibling.value != null && element_1_62_1.nextSibling.value != "" && element_1_62_1.nextSibling.value != " ")
   {
     ipAddrbuffer += element_1_62_1.nextSibling.value+",";
   }

   var element_1_63_1  =  getInputElementByXid('1_63_1');
   if(element_1_63_1 == null)
     return;
   if(element_1_63_1.nextSibling.value != "0.0.0.0" && element_1_63_1.nextSibling.value != null && element_1_63_1.nextSibling.value != "" && element_1_63_1.nextSibling.value != " ")
   {
     ipAddrbuffer += element_1_63_1.nextSibling.value+",";
   }

   var element_1_64_1  =  getInputElementByXid('1_64_1');
   if(element_1_64_1 == null)
     return;
   if(element_1_64_1.nextSibling.value != "0.0.0.0" && element_1_64_1.nextSibling.value != null && element_1_64_1.nextSibling.value != "" && element_1_64_1.nextSibling.value != " ")
   {
     ipAddrbuffer += element_1_64_1.nextSibling.value+",";
   }

   var element_1_65_1  =  getInputElementByXid('1_65_1');
   if(element_1_65_1 == null)
     return;
   if(element_1_65_1.nextSibling.value != "0.0.0.0" && element_1_65_1.nextSibling.value != null && element_1_65_1.nextSibling.value != "" && element_1_65_1.nextSibling.value != " ")
   {
     ipAddrbuffer += element_1_65_1.nextSibling.value+",";
   }

   var element_1_66_1  =  getInputElementByXid('1_66_1');
   if(element_1_66_1 == null)
     return;
   if(element_1_66_1.nextSibling.value != "0.0.0.0" && element_1_66_1.nextSibling.value != null && element_1_66_1.nextSibling.value != "" && element_1_66_1.nextSibling.value != " ")
   {
     ipAddrbuffer += element_1_66_1.nextSibling.value+",";
   }

   var element_1_67_1  =  getInputElementByXid('1_67_1');
   if(element_1_67_1 == null)
     return;
   if(element_1_67_1.nextSibling.value != "0.0.0.0" && element_1_67_1.nextSibling.value != null && element_1_67_1.nextSibling.value != "" && element_1_67_1.nextSibling.value != " ")
   {
     ipAddrbuffer += element_1_67_1.nextSibling.value;
   }
   if(ipAddrbuffer == null)
    return;

   var length = ipAddrbuffer.length;
   var temp = ipAddrbuffer.lastIndexOf(",");

   var element_1_68_1 = getInputElementByXid('1_68_1');
   if(element_1_68_1 == null)
    return;

   if(temp == (length-1))
   {
     element_1_68_1.value = ipAddrbuffer.substring(0,length-1);
   }
   else
   { 
     element_1_68_1.value = ipAddrbuffer;
   }

}

function DHCP_PoolCfg_SubmitEnd(submitFlag)
{
}

function DHCP_PoolCfg_LoadStart(arg)
{
//Decide the lease time 	
   var poolname1 = getInputElementByXid('1_2_1');
   var poolname2 = getInputElementByXid('1_111_1');
   var leaseTime = getInputElementByXid('1_19_1');
   var leaseTimeSelect =  getInputElementByXid('1_15_1'); 
   if(poolname1 != null)
   {
     if(poolname1.value == null || poolname1.value == "" || poolname1.value == " ")
     {
       leaseTimeSelect.value = "Infinite";
     }
     else
     { 
       if(leaseTime.value >= 86400)
         leaseTimeSelect.value = "Infinite";
       else
	   {
         leaseTimeSelect.value = "Specified Duration";	
       }		 
     }
   }
   if(poolname2 != null)
   {
     if(poolname2.value == null || poolname2.value == "" || poolname2.value == " ")
     {
       leaseTimeSelect.value = "Infinite";
     }
     else
     { 
       if(leaseTime.value >= 86400)
         leaseTimeSelect.value = "Infinite";
		 
       else
	    {
	     leaseTimeSelect.value = "Specified Duration";	   
       }	 
	 }
   }

   if(leaseTimeSelect.value == "Specified Duration")
   {
		  /*extract the days,hours and minutes from the lease time*/
          var days = leaseTime.value/1440;
          var mindue = leaseTime.value%1440;
          var hours =  mindue/60;
          var minutes = mindue%60;
          days = parseInt(days);
          hours = parseInt(hours);
          /*leaseTime.value = leaseTime.value%60;*/
          /*assign the days*/
          var element = getInputElementByXid('1_16_1');
          if(element == null)
             return;
          element.value= days;

          /*assign the hours*/
          var element = getInputElementByXid('1_17_1');
          if(element == null)
             return;
          element.value= hours;

          /*assign minutes*/
          var element = getInputElementByXid('1_18_1');
          if(element == null)
             return;
          element.value= minutes;

   }
/*Get the Default Router Addresses*/
   var element = getInputElementByXid('1_29_1');
   
   if(element.value == null || element.value == "" || element.value == " ")
   {
      element.value = "0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0";
   }
	   
   var range_of_values = element.value.split(",");
   
   if(range_of_values.length != 8) return;
   
   var element_1_21_1  =  getInputElementByXid('1_21_1');
   var element_1_22_1  =  getInputElementByXid('1_22_1');
   var element_1_23_1  =  getInputElementByXid('1_23_1');
   var element_1_24_1  =  getInputElementByXid('1_24_1');
   var element_1_25_1  =  getInputElementByXid('1_25_1');
   var element_1_26_1  =  getInputElementByXid('1_26_1');
   var element_1_27_1  =  getInputElementByXid('1_27_1');
   var element_1_28_1  =  getInputElementByXid('1_28_1');

   element_1_21_1.value = range_of_values[0];
   
   element_1_22_1.value = range_of_values[1];

   element_1_23_1.value = range_of_values[2]; 

   element_1_24_1.value = range_of_values[3];
  
   element_1_25_1.value = range_of_values[4];

   element_1_26_1.value = range_of_values[5];

   element_1_27_1.value = range_of_values[6];

   element_1_28_1.value = range_of_values[7];

/*Get the DNS Server Addresses*/

   var element = getInputElementByXid('1_39_1');
   
   if(element.value == null || element.value == "" || element.value == " ")
   {
      element.value = "0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0";
   }
	   
   var range_of_values = element.value.split(",");
   
   if(range_of_values.length != 8) return;
   
   var element_1_31_1  =  getInputElementByXid('1_31_1');
   var element_1_32_1  =  getInputElementByXid('1_32_1');
   var element_1_33_1  =  getInputElementByXid('1_33_1');
   var element_1_34_1  =  getInputElementByXid('1_34_1');
   var element_1_35_1  =  getInputElementByXid('1_35_1');
   var element_1_36_1  =  getInputElementByXid('1_36_1');
   var element_1_37_1  =  getInputElementByXid('1_37_1');
   var element_1_38_1  =  getInputElementByXid('1_38_1');

   element_1_31_1.value = range_of_values[0];
   
   element_1_32_1.value = range_of_values[1];

   element_1_33_1.value = range_of_values[2]; 

   element_1_34_1.value = range_of_values[3];
  
   element_1_35_1.value = range_of_values[4];

   element_1_36_1.value = range_of_values[5];

   element_1_37_1.value = range_of_values[6];

   element_1_38_1.value = range_of_values[7];


/*Get NETBIOS Server Addresses*/

   var element = getInputElementByXid('1_49_1');
   
   if(element.value == null || element.value == "" || element.value == " ")
   {
      //alert("3.Entering here...");	   
      element.value = "0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0";
   }
	   
   var range_of_values = element.value.split(",");
   
   if(range_of_values.length != 8) return;
   
   var element_1_41_1  =  getInputElementByXid('1_41_1');
   var element_1_42_1  =  getInputElementByXid('1_42_1');
   var element_1_43_1  =  getInputElementByXid('1_43_1');
   var element_1_44_1  =  getInputElementByXid('1_44_1');
   var element_1_45_1  =  getInputElementByXid('1_45_1');
   var element_1_46_1  =  getInputElementByXid('1_46_1');
   var element_1_47_1  =  getInputElementByXid('1_47_1');
   var element_1_48_1  =  getInputElementByXid('1_48_1');

   element_1_41_1.value = range_of_values[0];
   
   element_1_42_1.value = range_of_values[1];

   element_1_43_1.value = range_of_values[2]; 

   element_1_44_1.value = range_of_values[3];
  
   element_1_45_1.value = range_of_values[4];

   element_1_46_1.value = range_of_values[5];

   element_1_47_1.value = range_of_values[6];

   element_1_48_1.value = range_of_values[7];


/*Get IP address data values

   var element = getInputElementByXid('1_68_1');
   
   if(element.value == null || element.value == "" || element.value == " ")
   {
      //alert("4.Entering here...");	   
      element.value = "0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0,0.0.0.0";
   }
	   
   var range_of_values = element.value.split(",");
   
   if(range_of_values.length != 8) return;
   
   var element_1_60_1  =  getInputElementByXid('1_60_1');
   var element_1_61_1  =  getInputElementByXid('1_61_1');
   var element_1_62_1  =  getInputElementByXid('1_62_1');
   var element_1_63_1  =  getInputElementByXid('1_63_1');
   var element_1_64_1  =  getInputElementByXid('1_64_1');
   var element_1_65_1  =  getInputElementByXid('1_65_1');
   var element_1_66_1  =  getInputElementByXid('1_66_1');
   var element_1_67_1  =  getInputElementByXid('1_67_1');

   element_1_60_1.value = range_of_values[0];
   
   element_1_61_1.value = range_of_values[1];

   element_1_62_1.value = range_of_values[2]; 

   element_1_63_1.value = range_of_values[3];
  
   element_1_64_1.value = range_of_values[4];

   element_1_65_1.value = range_of_values[5];

   element_1_66_1.value = range_of_values[6];

   element_1_67_1.value = range_of_values[7];
*/
}

function DHCP_PoolCfg_LoadEnd(arg)
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

