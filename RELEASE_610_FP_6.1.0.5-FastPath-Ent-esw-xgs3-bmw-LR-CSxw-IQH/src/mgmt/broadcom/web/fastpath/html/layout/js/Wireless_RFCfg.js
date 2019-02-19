xuiIncludeJSFile("/scripts/global.js");
function Wireless_RFCfg_SubmitStart(submitFlag)

{

//alert("submit start...");

   var channel_planTime = getInputElementByXid('1_6_1');

   var temp;

   var element = getInputElementByXid('1_5_1');

   if(element == null || element.value == null) return;

   var channel_planTime_hours = element.nextSibling.value; 

   if(channel_planTime_hours > 24)
   {
  alert("Error! Value("+channel_planTime_hours+") is out of range for Channel Plan Fixed Time(hh:mm). The valid range is 0 to 24.");

  return;

}

   element = getInputElementByXid('1_5_2');

   if(element == null || element.value == null) return;

   var channel_planTime_minutes = element.nextSibling.value;

if(channel_planTime_minutes > 59)
{

  alert("Error! Value("+channel_planTime_minutes+") is out of range for Channel Plan Fixed Time(hh:mm). The valid range is 0 to 59.");

  return;

}

   temp = parseInt(channel_planTime_hours * 60)+ parseInt(channel_planTime_minutes);

   channel_planTime.value = temp;    

}

function Wireless_RFCfg_SubmitEnd(submitFlag)
{

}

function Wireless_RFCfg_LoadStart(arg)
{
//alert("Load Start");
xeData.nTabInfo = [["Configuration","RFConfiguration.html"],["Channel Plan History","ChannelPlanHistory.html"],["Manual Channel Plan","ManualChannelPlan.html"],["Manual Power Adjustments","ManualPowerAdjustments.html"]];

  var channel_planTime = getInputElementByXid('1_6_1');

  if(channel_planTime == null) return;

  if(channel_planTime.value == "" || channel_planTime.value == " "|| channel_planTime.value == null)

	  return;

  var hours = Math.floor(channel_planTime.value / 60);

  var minutes = channel_planTime.value % 60;

   var element = getInputElementByXid('1_5_1');

   if(element == null)

	   return;

   element.value = hours;

    var element = getInputElementByXid('1_5_2');

    if(element == null)

	    return;

    element.value = minutes;

}

function Wireless_RFCfg_LoadEnd(arg)
{
  loadme();
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

