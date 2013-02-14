/* Global Variables */
var lagSlotNum = 0;
emweb:/USMWEB_LAG_SLOT_NUM;
/* Global Variables */

function xuiRepeatInfoReadByCount(inputDIVName, columns)
{
  var inputDIV = document.getElementById(inputDIVName);

  allCAPTION = inputDIV.getElementsByTagName("CAPTION");
  var caption = allCAPTION[0].innerHTML;

  allTH = inputDIV.getElementsByTagName("TH");
  var headings = xuiGridTableHeaderArrayGet(allTH);

  // compute the max column lengths
  var maxlens = new Array();
  for(var col = 0; col < headings.length; col++)
  {
    maxlens[col] = headings[col].length;
    for(var row = 0; row < rows; row++)
    {
      if(maxlens[col] < cells[row][col].value.length)
        maxlens[col] = cells[row][col].value.length;
    }
  }

  // compute rows count
  allINPUT = inputDIV.getElementsByTagName("INPUT");
  var cells = allINPUT.length;
  var rows = cells / columns;

  // load the cells
  var cells = new Array();
  for(var row = 0, off = 0; row < rows; row++)
  {
    cells[row] = new Array();
    for(var col = 0; col < columns; col++, off++)
    {
      cells[row][col] = allINPUT[off];
    }
  }

  var repeatInfo = new Array();

  repeatInfo['cells'] = cells;
  repeatInfo['columns'] = columns;
  repeatInfo['rows'] = rows;
  repeatInfo['indiv'] = inputDIV;
  repeatInfo['id'] = inputDIVName;
  repeatInfo['headings'] = headings;
  repeatInfo['maxlens'] = maxlens;
  repeatInfo['caption'] = caption;

  return repeatInfo;
}

var refData = null;

function drawViewTemplate(data)
{
  var noOfGigPorts;
	
  var is48PortBox = false;
  var totalNoOfPorts;
	
	var noOfUnits = data.units.length;
	var noOfPorts = data.ports.length;
	var noOfLags  = data.lags.length;
  var unitTagID = data.unitIdx;
  var viewDisp  = data.viewType;
  var unitID    = data.units;
  // This is for LAG Split 
  var splitPortChannel = 12;

  refData = data;
	
	if (noOfPorts == 52 || noOfPorts == 28)
		 noOfGigPorts = 4;
	else
		 noOfGigPorts = 0;
	 
	noOfPorts = noOfPorts - noOfGigPorts;
	totalNoOfPorts = noOfPorts;

  if (noOfPorts > 24)
  {
    noOfPorts = noOfPorts/2;
    is48PortBox = true;
  }
  
  var htmlBuf = "";   

  
  if (noOfPorts > 0)
  {
    /* Port View Generation */
    //Table 1 Start
    htmlBuf = htmlBuf + '<table cellspacing="0" cellpadding="0" border="0" bgcolor="#eeeeee" id="table_ports" style="visibility: visible;display: table"><tbody><tr align="left" style="font-size: 12px;"><td bgcolor="white"/><td colspan="12">Unit<!--unit--></td></tr><tr align="center"><td bgcolor="white"/>';
    
    for (i=0 ; i<noOfUnits ; i++)
    {
      htmlBuf = htmlBuf + '<td><a style="text-decoration:none" href="javascript:selectAnotherUnit('+unitID[i]+',\''+unitTagID+'\')">'+unitID[i]+'</a></td>';
    }
  
    htmlBuf = htmlBuf + '</tr><tr align="left" style="font-size: 12px;"><td bgcolor="white"/><td colspan="24">Port<!--Port--></td></tr><tr align="center" style="font-size: 10px;"><td bgcolor="white"/>';

    for (i = 0; i < (noOfPorts + noOfGigPorts); i++)
    {
      if (i%12 == 0 && i !=0)
        htmlBuf = htmlBuf + '<td width="20px"></td>\n';
      if (i < noOfPorts)
        htmlBuf = htmlBuf + '\n<td width="20px">'+(i+1)+'</td>';
      else
        htmlBuf = htmlBuf + '\n<td width="20px">XG'+(i-23)+'</td>';
    }

    if (viewDisp == "read-write")
      htmlBuf = htmlBuf+'</tr><tr style="font-size: 10px;" id="static_ports_1_28"><td bgcolor="white">Static </td>';
    else
      htmlBuf = htmlBuf+'</tr><tr style="font-size: 10px;" id="static_ports_1_28"><td bgcolor="white"></td>';

    for (i = 0; i < (noOfPorts + noOfGigPorts); i++)
    {
      if (i >= noOfPorts)
        printindex=24+i;
      else
        printindex = i;
 
      if (i%12 == 0 && i != 0)
        htmlBuf = htmlBuf + '<td width="20px"></td>\n';

      if ((is48PortBox == true) && (i >= noOfPorts))
      {
        htmlBuf = htmlBuf + '<td><img src="/images/b.gif" onclick="portHandleFunc(this,'+(i+24)+')" name="Port'+(printindex+1)+'" id="Port'+(printindex+1)+'" style="cursor:pointer;"></td>';    
      }
      else
      {
        htmlBuf = htmlBuf + '<td><img src="/images/b.gif" onclick="portHandleFunc(this,'+i+')" name="Port'+(i+1)+'" id="Port'+(i+1)+'" style="cursor:pointer;"></td>';    
      }
    }
  
    if (viewDisp == "read-write")
    {
      htmlBuf = htmlBuf+'</tr><tr style="font-size: 10px;" id="current_ports_1_28"><td bgcolor="white">Current </td>';
 
      for (i = 0 ; i < (noOfPorts + noOfGigPorts); i++)
      {
        if (i >= noOfPorts)
          printindex=24+i;
        else
          printindex = i;
 
        if (i%12 == 0 && i != 0)
          htmlBuf = htmlBuf + '<td width="20px"></td>\n';
       
        if ((is48PortBox == true) && (i >= noOfPorts))
        {
          htmlBuf = htmlBuf+'<td><img src="/images/b.gif" name="CurrentPort'+(printindex+1)+'" id="CurrentPort'+(printindex+1)+'"></td>\n';
        }
        else
        {
          htmlBuf = htmlBuf+'<td><img src="/images/b.gif" name="CurrentPort'+(i+1)+'" id="CurrentPort'+(i+1)+'"></td>\n';
        }
      }  
    }
    else
    {
      htmlBuf = htmlBuf+'</tr><tr style="font-size: 10px;" id="current_ports_1_28"><td bgcolor="white"></td>';
    }

    htmlBuf = htmlBuf+'</tr><tr><td bgcolor="white"/><td></td></tr></tbody></table>';
    //Table 1 End

    htmlBuf = htmlBuf + '<br>';
  
    if (is48PortBox == true)
    {
      //Table 2  Start
  
      htmlBuf = htmlBuf + '<table cellspacing="0" cellpadding="0" border="0" bgcolor="#eeeeee" id="table_ports_48" style="visibility: visible;display: table"><tbody><tr align="left" style="font-size: 12px;"><td bgcolor="white"/><td colspan="24">Port<!--Port--></td></tr><tr align="center" style="font-size: 10px;"><td bgcolor="white"/>';

      for (i = noOfPorts; i < totalNoOfPorts ; i++)
      {
        if (i == 36)
          htmlBuf = htmlBuf + '<td width=20px></td>\n';
        htmlBuf = htmlBuf + '\n<td width=20px>'+(i+1)+'</td>';
      }

      if (viewDisp == "read-write")
        htmlBuf = htmlBuf+'</tr><tr style="font-size: 10px;" id="static_ports_25_48"><td bgcolor="white">Static </td>';
      else
        htmlBuf = htmlBuf+'</tr><tr style="font-size: 10px;" id="static_ports_25_48"><td bgcolor="white"></td>';

      for (i = noOfPorts; i < totalNoOfPorts ; i++)
      {
        if (i == 36)
          htmlBuf = htmlBuf + '<td width=20px></td>\n'; 
        htmlBuf = htmlBuf + '<td><img src="/images/b.gif" onclick="portHandleFunc(this,'+i+')" name="Port'+(i+1)+'" id="Port'+(i+1)+'" style="cursor:pointer;"></td>'; 
      }


      if (viewDisp == "read-write")
      { 
        htmlBuf = htmlBuf + '</tr><tr style="font-size: 10px;" id="current_ports_25_48"><td bgcolor="white">Current </td>';

        for (i = noOfPorts; i < totalNoOfPorts ; i++)
        {
          if (i == 36)  
            htmlBuf = htmlBuf + '<td width=20px></td>\n'; 

          htmlBuf = htmlBuf + '<td><img src="/images/b.gif" name="CurrentPort'+(i+1)+'" id="CurrentPort'+(i+1)+'"></td>'; 
        }
      }
      else
      {
        htmlBuf = htmlBuf + '</tr><tr style="font-size: 10px;" id="current_ports_25_48"><td bgcolor="white"></td>';
      }

      htmlBuf = htmlBuf + '</tr></tbody></table>';
       
  
      //Table 2 End
    }
  }
  htmlBuf = htmlBuf + "<hr/>";  
	var portView = htmlBuf;


  /* LAG View Generation */
 
  htmlBuf = "";
  var totalNoOfLags = noOfLags;
   if(noOfLags > 24)
      noOfLags = noOfLags/2;

  if (noOfLags > 0)
  {
    //LAG Table Start
    htmlBuf = htmlBuf + '<table cellspacing="0" cellpadding="0" border="0" bgcolor="#eeeeee" id="table_lags" style="visibility: visible;display: table"><tbody><tr align="left" style="font-size: 12px;"><td bgcolor="white"/><td colspan="24">Lags</td></tr><tr align="center" style="font-size: 10px;"><td bgcolor="white"/>';

    for (i = 0; i < noOfLags ; i++)
    {
        if (i%(splitPortChannel) == 0 && i !=0)
            htmlBuf = htmlBuf + '<td width="20px"></td>\n';
      htmlBuf = htmlBuf + '\n<TD WIDTH=20px>'+(i+1)+'</TD>';
    }

    if (viewDisp == "read-write")
    {
      htmlBuf = htmlBuf + '</tr><tr style="font-size: 10px;" id="static_lags_1_48"><td bgcolor="white">Static </td>';
    }
    else 
    {
      htmlBuf = htmlBuf + '</tr><tr style="font-size: 10px;" id="static_lags_1_48"><td bgcolor="white"></td>';
    }

    for ( i = 0 ; i < noOfLags ; i++)
    {
        if (i%(splitPortChannel) == 0 && i !=0)
            htmlBuf = htmlBuf + '<td width="20px"></td>\n';
      htmlBuf = htmlBuf + '<td><img src="/images/b.gif" onclick="lagHandleFunc(this,'+i+')" name="LagPort'+(i+1)+'" ID="LagPort'+(i+1)+'" style="cursor:pointer;\"></td>'; 
    }

    if (viewDisp == "read-write")
    {
      htmlBuf = htmlBuf + '</tr><tr style="font-size: 10px;" id="current_lags_1_48"><td bgcolor="white">Current </td>';

      for ( i = 0 ; i < noOfLags ; i++)
      {
        if (i%(splitPortChannel) == 0 && i !=0)
          htmlBuf = htmlBuf + '<td width="20px"></td>\n';
        htmlBuf = htmlBuf + '<td><img src="/images/b.gif" name="CurrentLagPort'+(i+1)+'" id="CurrentLagPort'+(i+1)+'" style="cursor:pointer;"></td>'; 
      }
    }
    else
    {
      htmlBuf = htmlBuf + '</tr><tr style="font-size: 10px;" id="current_lags_1_48"><td bgcolor="white"></td>';
    }
 
    htmlBuf = htmlBuf + '</tr></tbody></table>';

     htmlBuf = htmlBuf + '<br>';
    if(totalNoOfLags == 48)
    {
         htmlBuf = htmlBuf + '<table cellspacing="0" cellpadding="0" border="0" bgcolor="#eeeeee" id="table_lags" style="visibility: visible;display: table"><tbody><tr align="left" style="font-size: 12px;"><td bgcolor="white"/><td colspan="24">Lags</td></tr><tr align="center" style="font-size: 10px;"><td bgcolor="white"/>';

       for (i = noOfLags; i < totalNoOfLags ; i++)
       {
            if (i == 36)  
           htmlBuf = htmlBuf + '<td width="20px"></td>\n';
          htmlBuf = htmlBuf + '\n<TD WIDTH=20px>'+(i+1)+'</TD>';
       }

       if (viewDisp == "read-write")
       {
          htmlBuf = htmlBuf + '</tr><tr style="font-size: 10px;" id="static_lags_25_48"><td bgcolor="white">Static </td>';

      }
       else
       {
         htmlBuf = htmlBuf + '</tr><tr style="font-size: 10px;" id="static_lags_25_48"><td bgcolor="white"></td>';
       }
       for ( i = noOfLags ; i < totalNoOfLags ; i++)
       {
          if (i == 36)               
             htmlBuf = htmlBuf + '<td width="20px"></td>\n';
           htmlBuf = htmlBuf + '<td><img src="/images/b.gif" onclick="lagHandleFunc(this,'+i+')" name="LagPort'+(i+1)+'" ID="LagPort'+(i+1)+'" style="cursor:pointer;\"></td>';
       }
 
       if (viewDisp == "read-write")
       {
           htmlBuf = htmlBuf + '</tr><tr style="font-size: 10px;" id="current_lags_25_48"><td bgcolor="white">Current </td>';

           for ( i = noOfLags ; i < totalNoOfLags ; i++)
           {   if (i == 36)
                 htmlBuf = htmlBuf + '<td width="20px"></td>\n';
                 htmlBuf = htmlBuf + '<td><img src="/images/b.gif" name="CurrentLagPort'+(i+1)+'" id="CurrentLagPort'+(i+1)+'" style="cursor:pointer;"></td>';
           }
       }
       else
       {
           htmlBuf = htmlBuf + '</tr><tr style="font-size: 10px;" id="current_lags_25_48"><td bgcolor="white"></td>';
       }

       htmlBuf = htmlBuf + '</tr></tbody></table>';

    }

    //LAG Table End
  }

	var lagView = htmlBuf;
	
  //document.getElementById('plotView').innerHTML = portView+lagView;
  data.plotDIV.innerHTML = portView+lagView;  

}

function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if(element.getAttribute('xid') == null ||
           element.getAttribute('xid') != xid) continue;

        return element;
    }
}

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

function selectAnotherUnit(selectedUnit)
{
  var inputElmns = document.getElementsByTagName('input');
  for (var i=0; i<inputElmns.length; i++)
  {
    if (inputElmns[i].getAttribute('XC') == null || 
        inputElmns[i].getAttribute('XC') != 'hidden')
    {
      inputElmns[i].disabled = true; 
    }
  }

  getInputElementByXid(refData.unitIdx).disabled = false;
  getInputElementByXid(refData.unitIdx).value = selectedUnit;
  document.forms[0].submit();
}

/* Converts U/S/P to U, S, P */
function uspSplit(usp)
{
  var uspArr = new Array()
  uspArr = usp.split("/");

  return uspArr;
}

/* Converts U, S, P to U/S/P */
function getUSP(port, intIfType)
{
  var unit = 0; 
  var slot = 0;

  if (intIfType == 'Lag')
  {
    unit = 0;
    slot = lagSlotNum;
  }
  else
  {
    unit = viewData.selUnit.value;
    slot = 0;
  }

  return (unit+'/'+slot+'/'+port);
}

/*Apply background to selected unit*/
function highlightSelUnit(data)
{
  var selUnitIdx = 0;
  for (var i=0; i<data.units.length; i++)
  {
    if (data.selUnit.value == data.units[i])
      selUnitIdx = i;
  }

  var tableObj = document.getElementById("table_ports");
  if (tableObj != null)
    tableObj.rows[1].cells[selUnitIdx+1].bgColor='#9e9e9e';

  return;
}

function sliceString(stringValue,token)
{
  var arr=new Array();
  arr=stringValue.split(token);
  return arr;
}

function IPToMulticastMac(ipAddr)
{
  var addrType = 'Multicast';

  if (ipAddr.length == 0)
    return -1;

  if (validIPv4(ipAddr, addrType) == false)
    return -1;

  var macStr="01005e";
  ipArr=sliceString(ipAddr,".");
  var isCont=true;
  if ((parseInt(ipArr[0])<224) || (parseInt(ipArr[0])>239))
  {
    isCont=false;
    alert("Multicat Mac address must be between 224.0.0.0-239.255.255.255");
  }
  if (isCont)
  {
    ipArr[1]=(ipArr[1]&127).toString();
    for (var i=1; i<4; i++)
    {
      ipArr[i]=Str2Hex(ipArr[i]);
      if (ipArr[i]==0 || ipArr[i]=="0")
        ipArr[i]="00";
      if (ipArr[i].length==1)
        ipArr[i]="0"+ipArr[i];
      macStr+=ipArr[i];
    }
    macStr=StringToMacAddress(macStr);
    return macStr;
  }
  else 
    return -1;
}

function StringToMacAddress(stringVal)
{
  var macAddressVal="";
  var splitedString = sliceString(stringVal,"");
  for(k=0; k<splitedString.length; k++)
  {
    if(((k+1)%2!=0)||(k==splitedString.length-1))
      macAddressVal = macAddressVal+splitedString[k];
    else
      macAddressVal = macAddressVal+splitedString[k]+":";
  }
  return macAddressVal;
}


function Str2Hex(str)
{
  origin=10;
  dest=16;
  input=""+str;
  base="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  c=0; Result="";
  for (t=1; t<=input.length; t++)
  {
    b=base.indexOf(input.substring(t-1,t));
    n=b*(Math.pow (origin, input.length-t));
    c+=n;
  }
  a=100;
  while (c < Math.pow (dest, a)) 
  { 
    a--; 
  }
  while (a>-1)
  {
    e=Math.pow (dest, a);
    a--;
    d=((c-(c%e))/e)+1;
    c=c%e; Ciffer=base.substring(d-1, d);
    Result = Result + Ciffer;
  }
  return Result;
}

function validIPv4(ipAddr, addrType) 
{
  var validChars = '.0123456789';
  if (!ipAddr)
    return false;

  dots = 0;

  for (var i = 0; i < ipAddr.length; i++) 
  {
    var chr = ipAddr.substring(i,i+1);
    if (validChars.indexOf(chr) == -1)
      return false;
    if (chr == '.')  
    {
      dots++;
      eval('dot' + dots + ' = ' + i);
    }
  }

  if (dots != 3)
    return false;

  if (ipAddr.substring(0,1) == '.' || ipAddr.substring(ipAddr.length,ipAddr.length+1) == '.')
    return false;

  ip1 = ipAddr.substring(0,dot1);
  if (!ip1 || ip1 >255)
    return false;
  ip2 = ipAddr.substring(dot1+1,dot2);
  if (!ip2 || ip2 >255)
    return false;
  ip3 = ipAddr.substring(dot2+1,dot3);
  if (!ip3 || ip3 >255)
    return false;
  ip4 = ipAddr.substring(dot3+1,ipAddr.length+1);
  if (!ip4 || ip4 >255)
    return false;

  if (ip1 == 0 && ip2 == 0 && ip3 == 0 && ip4 == 0)
    return false;
    
  if (addrType == 'Multicast') 
  {
    if ((ip1 >= 224 && ip1 <= 239) && (ip2 >= 0 && ip2 <=255) && (ip3 >= 0 && ip3 <=255) && (ip4 >= 0 && ip4 <=255))
      return true
    else
      return false;
  }
  else
  {
    return true;
  }       
}


function check(args)
{
 var Ids = args.split("|");
 var elem_2_6_1 = getInputElementByXid(Ids[0]);
 var elem_2_6_2 = getInputElementByXid(Ids[1]);
 var elem_2_6_3 = getInputElementByXid(Ids[2]);
 var elem_2_6_4 = getInputElementByXid(Ids[3]);
 var elem_3_1_2 = getInputElementByXid(Ids[4]);
 var elem_3_1_3 = getInputElementByXid(Ids[5]);
 var elem_3_1_5 = getInputElementByXid(Ids[6]);
 var elem_3_2_2 = getInputElementByXid(Ids[7]);
 var elem_3_3_2 = getInputElementByXid(Ids[8]);

 if (elem_2_6_1.nextSibling.checked == true && elem_2_6_4.nextSibling.checked == false){
  elem_2_6_2.nextSibling.checked=true;
  elem_2_6_2.nextSibling.disabled=false;
  elem_2_6_3.nextSibling.checked=true;
  elem_2_6_3.nextSibling.disabled=false;

}
else{
  elem_2_6_2.nextSibling.checked=false;
 // elem_2_6_2.nextSibling.disabled=true;
  elem_2_6_3.nextSibling.checked=false;
  elem_2_6_3.nextSibling.disabled=true;
  if (elem_3_1_2 != null)
    elem_3_1_2.nextSibling.checked=false;
  if (elem_3_1_3 != null)
    elem_3_1_3.nextSibling.checked=false;
  if (elem_3_1_5 != null)
    elem_3_1_5.nextSibling.checked=false;
  if (elem_3_2_2 != null)
    elem_3_2_2.nextSibling.checked=false;
  if (elem_3_3_2 != null)
    elem_3_3_2.nextSibling.checked=false;
}

}

function protoCheckIPv6Rule(args)
{
 var Ids = args.split("|");
 var elem_2_6_1 = getInputElementByXid(Ids[0]);
 var elem_2_6_2 = getInputElementByXid(Ids[1]);
 var elem_2_6_3 = getInputElementByXid(Ids[2]);
 var elem_2_6_4 = getInputElementByXid(Ids[3]);

 if (elem_2_6_1.nextSibling.checked == true && elem_2_6_4.nextSibling.checked == false){
  elem_2_6_2.nextSibling.checked=true;
  elem_2_6_2.nextSibling.disabled=false;
  elem_2_6_3.nextSibling.checked=true;
  elem_2_6_3.nextSibling.disabled=false;

}
else{
  elem_2_6_2.nextSibling.checked=false;
 // elem_2_6_2.nextSibling.disabled=true;
  elem_2_6_3.nextSibling.checked=false;
  elem_2_6_3.nextSibling.disabled=true;
}

}


function showUpdateinfo(args)
{
 var Ids = args.split("|");
 var fileType = getInputElementByXid(Ids[0]);
 var displayStatus = getInputElementByXid(Ids[1]);
 var displayStatusParentTD;

 if (displayStatus != undefined)
 {
   displayStatusParentTD = displayStatus.parentNode;
 }
 else
 {
   displayStatusParentTD = document.getElementById('2_6_1');
 }

 var displayStr = ""; 
 
 var fileTypeVisible = fileType.nextSibling;
 if( fileTypeVisible.selectedIndex == 0)
 {
   //Firmware
   displayStr = "Upload Firmware image";
 } 
 if( fileTypeVisible.selectedIndex == 1)
 {
   //Configuration
   displayStr = "Upload Configuration File";
 } 
 if( fileTypeVisible.selectedIndex == 2)
 {
   //Startup Log
   displayStr = "Upload Startup Log file";
 } 
 if( fileTypeVisible.selectedIndex == 3)
 {
   //Operational Log
   displayStr = "Upload Operational Log file";
 } 

 var displayStrObj = document.createTextNode(displayStr);

 displayStatusParentTD.innerHTML = "";
 displayStatusParentTD.appendChild(displayStrObj,null);
 
}

function showMessage()
{
//var elem_2_5_1 = getInputElementByXid(args);
//if (elem_2_5_1.nextSibling.checked == false)
alert("Match Every or Protocol must be selected to configure a rule");
}

