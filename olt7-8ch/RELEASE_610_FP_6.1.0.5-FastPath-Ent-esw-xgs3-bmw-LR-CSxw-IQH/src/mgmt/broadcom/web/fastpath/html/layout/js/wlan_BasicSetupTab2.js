xuiIncludeJSFile("/scripts/global.js");
function wlan_BasicSetupTab2_SubmitStart(submitFlag)
{
  selectListItems();
  var element_1_2_1 = getInputElementByXid('1_2_1');
  var element_2_2_1 = getInputElementByXid('2_2_1');
  if(element_1_2_1 != null)
  {  
    list = element_1_2_1.nextSibling;
  } 
  if(element_2_2_1 != null)
  {
    list = element_2_2_1.nextSibling;
  } 
  /*Add the ip list options to the object*/
  var element_1_12_2  =  getInputElementByXid('1_12_2');
  var element_2_12_2  =  getInputElementByXid('2_12_2');
  if(element_1_12_2 != null)
  {
    element_1_12_2.nextSibling.value = "";
  }
  if(element_2_12_2 != null)
  {
    element_2_12_2.nextSibling.value = "";
  }
  for (i=0;i<list.length;i++)
  {
    if(element_1_12_2 != null)
    {
    element_1_12_2.nextSibling.value += list.options[i].text;
    element_1_12_2.nextSibling.value += ",";
    }
    if(element_2_12_2 != null)
    {
    element_2_12_2.nextSibling.value += list.options[i].text;
    element_2_12_2.nextSibling.value += ",";
    } 
  }
 
  var element_1_2_2 = getInputElementByXid('1_2_2');
  var element_2_2_2 = getInputElementByXid('2_2_2');
  if(element_1_2_2 != null)
  {
    list = element_1_2_2.nextSibling;
  }
  if(element_2_2_2 != null)
  {
    list = element_2_2_2.nextSibling;
  }
  /*Add the vlan list options to the object*/
  var element_1_13_2  =  getInputElementByXid('1_13_2');
  var element_2_13_2  =  getInputElementByXid('2_13_2');
  if(element_1_13_2 != null)
  {
    element_1_13_2.nextSibling.value = "";
  }
  if(element_2_13_2 != null)
  {
    element_2_13_2.nextSibling.value = "";
  }
  for (i=0;i<list.length;i++)
  {
    myArray = list.options[i].text.split(" -");
    if(element_1_13_2 != null)
    {
    element_1_13_2.nextSibling.value += myArray[0];
    element_1_13_2.nextSibling.value += ",";
    }
    if(element_2_13_2 != null)
    {
    element_2_13_2.nextSibling.value += myArray[0];
    element_2_13_2.nextSibling.value += ",";
    }
  } 
}
function wlan_BasicSetupTab2_SubmitEnd(submitFlag)
{
}

function wlan_BasicSetupTab2_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Global","WirelessGlobalConfig.html"], ["Discovery","WirelessDiscoveryConfig.html"], ["Profile","WirelessDefaultProfileConfig.html"], ["Radio","WirelessDefaultRadioConfiguration.html"], ["VAP","WirelessDefaultVAPConfig.html"], ["Valid AP","local_ap_database_summ.html"], ["OUI","local_oui_database_summ.html"] ];

}

function wlan_BasicSetupTab2_LoadEnd(arg)
{
  loadme();

/*Clear the IP Address*/
  var ip;
  ip_rw = getInputElementByXid('1_3_1');
  if(ip_rw != null)
  { 
    ip_rw = getInputElementByXid('1_3_1').nextSibling;
    ip_rw.value="";
    ip = ip_rw.value;
  }
  ip_ro = getInputElementByXid('2_3_1');
  if(ip_ro != null)
  {
    ip_ro = getInputElementByXid('2_3_1').nextSibling;
    ip_ro.value="";
    ip = ip_ro.value;
  }
 
  /*Get the IP List and set the size*/
  var iplist;
  iplist_rw = getInputElementByXid('1_2_1');
  if(iplist_rw != null) 
  { 
    iplist_rw = getInputElementByXid('1_2_1').nextSibling;
    iplist = iplist_rw;
  }
  iplist_ro = getInputElementByXid('2_2_1');
  if(iplist_ro != null)
  {
    iplist_ro = getInputElementByXid('2_2_1').nextSibling;
    iplist = iplist_ro;
  }
  iplist.size = 10;

/*Get the IP Addresses*/
  var ipaddrs;
  ipaddrs_rw = getInputElementByXid('1_12_1');
  if(ipaddrs_rw != null) 
  {
    ipaddrs_rw = getInputElementByXid('1_12_1').nextSibling;
    ipaddrs = ipaddrs_rw;
  }
  ipaddrs_ro = getInputElementByXid('2_12_1');
  if(ipaddrs_ro != null)
  {
    ipaddrs_ro = getInputElementByXid('2_12_1').nextSibling;
    ipaddrs = ipaddrs_ro;
  }

/*Add Blank to the IP List, if empty*/
  if(iplist.options[0].text == "" || iplist.options[0].text == " " || iplist.options[0].text == "Disable" || ipaddrs.value == "" || ipaddrs.value == " ")
  {
    iplist.options[0] = null;
    addBlankToList("IP");
  }

/*Assign the configured IP addresses to the IP List*/ 
  if(ipaddrs.value != "" && ipaddrs.value != " ")
  {
    var range_of_values = ipaddrs.value.split(",");  
    for(i=0; i<range_of_values.length;i++)
    {
      if(range_of_values[i] != "")
      {
        val = range_of_values[i];
        iplist.options[i] = new Option(val);
      }
    } 
  }

/*Clear the vlan id*/
  var vlan;
  vlan_rw = getInputElementByXid('1_3_2');
  if(vlan_rw != null) 
  {
    vlan_rw = getInputElementByXid('1_3_2').nextSibling;
    vlan_rw.value="";
    vlan = vlan_rw.value;
  }
  vlan_ro = getInputElementByXid('2_3_2');
  if(vlan_ro != null)
  {
    vlan_ro = getInputElementByXid('2_3_2').nextSibling;
    vlan_ro.value="";
    vlan = vlan_ro.value;
  }

/*Get the Vlan List and set the size*/
  var vlanlist;
  vlanlist_rw = getInputElementByXid('1_2_2');
  if(vlanlist_rw != null)
  {
    vlanlist_rw = getInputElementByXid('1_2_2').nextSibling;
    vlanlist = vlanlist_rw;
  }
  vlanlist_ro = getInputElementByXid('2_2_2');
  if(vlanlist_ro != null)
  {
    vlanlist_ro = getInputElementByXid('2_2_2').nextSibling;
    vlanlist = vlanlist_ro;
  }
  vlanlist.size = 10;

/*Get the vlans*/
  var vlans;
  vlans_rw = getInputElementByXid('1_13_1');
  if(vlans_rw != null) 
  {
    vlans_rw = getInputElementByXid('1_13_1').nextSibling;
    vlans = vlans_rw;
  }
  vlans_ro = getInputElementByXid('2_13_1');
  if(vlans_ro != null)
  {
    vlans_ro = getInputElementByXid('2_13_1').nextSibling;
    vlans = vlans_ro;
  }

/*Add Blank to the Vlan List, if empty*/
  if(vlanlist.options[0].text == "" || vlanlist.options[0].text == " " || vlanlist.options[0].text == "Disable")
  {
    vlanlist.options[0] = null;
    addBlankToList("VLAN");
  }
  
/*Assign the configured vlans to the VLAN List*/
  if(vlans.value != "" && vlans.value != " ")
  {
    var range_of_values = vlans.value.split(",");
    for(i=0; i<range_of_values.length;i++)
    {
      if(range_of_values[i] != "")
      {
        val = range_of_values[i];
        vlanlist.options[i] = new Option(val);
      }
    }
  }

  var addBtn1_rw = document.getElementById("11_4_1");
  if(addBtn1_rw != null)
  {
    addBtn1_rw.align = "right";
  }
  
  var delBtn1_rw = document.getElementById("11_4_2");
  if(delBtn1_rw != null)
  {
    delBtn1_rw.align = "left"
  }

  var addBtn2_rw = document.getElementById("11_4_3");
  if(addBtn2_rw != null)
  {
    addBtn2_rw.align = "right";
  }
 
  var delBtn2_rw = document.getElementById("11_4_4");
  if(delBtn2_rw != null)
  {
    delBtn2_rw.align = "left"
  }

  var addBtn1_ro = document.getElementById("22_4_1");
  if(addBtn1_ro != null)
  {
    addBtn1_ro.align = "right";
  }
  
  var delBtn1_ro = document.getElementById("22_4_2");
  if(delBtn1_ro != null)
  {
    delBtn1_ro.align = "left"
  }

  var addBtn2_ro = document.getElementById("22_4_3");
  if(addBtn2_ro != null)
  {
    addBtn2_ro.align = "right";
  }

  var delBtn2_ro = document.getElementById("22_4_4");
  if(delBtn2_ro != null)
  {
    delBtn2_ro.align = "left"
  }

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
