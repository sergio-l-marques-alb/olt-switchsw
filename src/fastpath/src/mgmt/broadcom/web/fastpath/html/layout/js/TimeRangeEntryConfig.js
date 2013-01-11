xuiIncludeJSFile("/scripts/xui_oem.js");
function fp_EncodeStringRange(xid,list_values,conf_values)
{

  var tmpStr="";
  for(var index=0;index<conf_values.length;index++)
      tmpStr = tmpStr + conf_values[index] + ",";

  if(tmpStr.indexOf(",") == tmpStr.length -1)  {
     tmpStr = tmpStr.substring(0,tmpStr.length-1);
  }

  return tmpStr;
}

function fp_DecodeStringRange(xid,list_values,rangeStr){

  var  range=rangeStr.split(",");
  return getNonNullValues(range);

}

function joinEle(dummy_element, hours, minutes)
{
  var str = ":";


  if(dummy_element == null) return;
  if(hours == null) return;
  if(minutes == null) return;

  if( (hours.value == "") || (minutes.value == ""))
  {
    return;
  }
  else
  {
    dummy_element.nextSibling.value = "";
    dummy_element.nextSibling.value = dummy_element.nextSibling.value.concat(hours.value.concat(str.concat(minutes.value)));
  }
  return;
}

function splitEle(dummy_element, hours, minutes)
{
  var temp;

  if(dummy_element == null) return;
  if(hours == null) return;
  if(minutes == null) return;

  temp = dummy_element.nextSibling.value.split(":");

  hours.value = hours.value.replace(hours.value, temp[0]);
  minutes.value = minutes.value.replace(minutes.value, temp[1]);

  return;

}

function TimeRangeEntryConfig_SubmitStart()
{
  var pr_st_dummy_element = getInputElementByXid('1_22_1');
  var pr_st_hours = getInputElementByXid('1_22_2');
  var pr_st_minutes = getInputElementByXid('1_22_3');
  
  splitEle(pr_st_dummy_element, pr_st_hours, pr_st_minutes);
  
  var pr_ed_dummy_element = getInputElementByXid('1_33_1');
  var pr_ed_hours = getInputElementByXid('1_33_2');
  var pr_ed_minutes = getInputElementByXid('1_33_3');
  
  splitEle(pr_ed_dummy_element, pr_ed_hours, pr_ed_minutes);
 
  var ab_st_dummy_element = getInputElementByXid('1_94_1');
  var ab_st_hours = getInputElementByXid('1_94_2');
  var ab_st_minutes = getInputElementByXid('1_94_3');

  splitEle(ab_st_dummy_element, ab_st_hours, ab_st_minutes);
 
  var ab_ed_dummy_element = getInputElementByXid('1_114_1');
  var ab_ed_hours = getInputElementByXid('1_114_2');
  var ab_ed_minutes = getInputElementByXid('1_114_3');
  
  splitEle(ab_ed_dummy_element, ab_ed_hours, ab_ed_minutes);
}
 
function TimeRangeEntryConfig_SubmitEnd()
{
  
}
 
function TimeRangeEntryConfig_LoadStart()
{
  xuiTypeRegister("dayList_t", fp_EncodeStringRange, fp_DecodeStringRange);

  var ele23_2 = getInputElementByXid('1_23_2');
  var ele23_5  = getInputElementByXid('1_23_5');

  var temp="0";
  
  if(ele23_2.value.length==1)
  {
    ele23_2.value= temp.concat(ele23_2.value);
  }
 
  temp="0";   
  if(ele23_5.value.length==1)
  {
    ele23_5.value= temp.concat(ele23_5.value);
  }
  var ele34_2 = getInputElementByXid('1_34_2');
  var ele34_5  = getInputElementByXid('1_34_5');

  temp="0";

  if(ele34_2.value.length==1)
  {
    ele34_2.value= temp.concat(ele34_2.value);
  }

  temp="0";
  if(ele34_5.value.length==1)
  {
    ele34_5.value= temp.concat(ele34_5.value);
  }

  
  var ele41_5 = getInputElementByXid('1_41_5');
  var ele41_6  = getInputElementByXid('1_41_6');

  temp="0";
  if(ele41_5.value.length==1)
  {
    ele41_5.value= temp.concat(ele41_5.value);
  }

  temp="0";
  if(ele41_6.value.length==1)
  {
    ele41_6.value= temp.concat(ele41_6.value);
  }

  var ele51_5 = getInputElementByXid('1_51_5');
  var ele51_6  = getInputElementByXid('1_51_6');

    temp="0";
  if(ele51_5.value.length==1)
  {
    ele51_5.value= temp.concat(ele51_5.value);
  }

  temp="0";
  if(ele51_6.value.length==1)
  {
    ele51_6.value= temp.concat(ele51_6.value);
  }


} 

function TimeRangeEntryConfig_LoadEnd()
{
  var pr_st_dummy_element = getInputElementByXid('1_22_1');
  var pr_st_hours = getInputElementByXid('1_22_2');
  var pr_st_minutes = getInputElementByXid('1_22_3');

  joinEle(pr_st_dummy_element, pr_st_hours, pr_st_minutes);

  var pr_ed_dummy_element = getInputElementByXid('1_33_1');
  var pr_ed_hours = getInputElementByXid('1_33_2');
  var pr_ed_minutes = getInputElementByXid('1_33_3');

  joinEle(pr_ed_dummy_element, pr_ed_hours, pr_ed_minutes);

  var ab_st_dummy_element = getInputElementByXid('1_94_1');
  var ab_st_hours = getInputElementByXid('1_94_2');
  var ab_st_minutes = getInputElementByXid('1_94_3');

  joinEle(ab_st_dummy_element, ab_st_hours, ab_st_minutes);

  var ab_ed_dummy_element = getInputElementByXid('1_114_1');
  var ab_ed_hours = getInputElementByXid('1_114_2');
  var ab_ed_minutes = getInputElementByXid('1_114_3');

  joinEle(ab_ed_dummy_element, ab_ed_hours, ab_ed_minutes);
}
function selectEndDayonStartDaySelect()
{
  var sel_opt =0;
  var obj_117_1  = getInputElementByXid('1_117_1');
  var obj_25_1  = getInputElementByXid('1_25_1');   
  var obj_118_6  = getInputElementByXid('1_118_6');   
 
  var sel_117_1 = obj_117_1.nextSibling;
  var tmp_ary = new Array();

  var options117 = sel_117_1.options;

  var name = obj_117_1.name;
  var xid = obj_117_1.getAttribute('xid');

  for(var index=0;index<options117.length;index++)
  {
    if(options117[index].selected == true)
    {
      sel_opt++;
    }
  }
  
  if(sel_opt == 1)
  {
    //show enable  action on End Day 1_25_3
    xuiShed(0, name2Prefix(name),xid, "1_25_1");//show
    xuiShed(3, name2Prefix(name),xid, "1_25_1");//enable
    
    //disable  action on End Day 1_118_6
    xuiShed(2, name2Prefix(name),xid, "1_118_6");//disable
    return;
  }

  if(sel_opt > 1 )
  {
    for(var index=0;index<options117.length;index++)
    {
      if(options117[index].selected == true)
      {
        tmp_ary[tmp_ary.length] = options117[index].value;
      }
    }
  }

  if(tmp_ary.length ==0)
  {
    return;
  }
  
  xuiShed(3, name2Prefix(name),xid, "1_118_6");//enable 
  obj_118_6.value = xuiTypeEncode('1_25_1',tmp_ary);    
  //hide & disable  action on End Day 1_25_3
  xuiShed(1, name2Prefix(name),xid, "1_25_1"); //hide
  xuiShed(2, name2Prefix(name),xid, "1_25_1"); //disable
}
