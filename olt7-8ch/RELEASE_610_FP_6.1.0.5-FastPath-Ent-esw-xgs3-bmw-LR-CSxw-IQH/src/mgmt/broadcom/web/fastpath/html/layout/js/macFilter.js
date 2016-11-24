var fp_nullValue="$";
//return 0 if equal
//return -1 if lesser
//return 1 otherwise
function fp_getDiff(list_values,valOne,valTwo)
{

   var index1=0,index2=0;

   if(valOne == valTwo) return 0;

   for(var index=0;index<list_values.length;index++)   {
       if(list_values[index] == valOne) index1=index;
       if(list_values[index] == valTwo) index2=index;

   }
   

   return index1-index2;
}

function fp_getIncr(list_values,value)
{
  for(var index=0;index<list_values.length;index++){
      if(list_values[index] == value && index+1 < list_values.length) {
          return list_values[index+1];
      }
  }
  return value;
}

function fp_parseAndGetUpperLimit(range)
{

  if(range.indexOf("-") == -1) return range;

  var range_vals = range.split("-") ;
  return range_vals[1];
}

function fp_parseAndGetLowerLimit(range)
{
  if(range.indexOf("-") == -1) return range;

  var range_vals = range.split("-") ;
  return range_vals[0];
}

function fp_prepareRange(valOne,valTwo)
{
  var lowerLimit = parseAndGetLowerLimit(valOne) ;
  var upperLimit = parseAndGetUpperLimit(valTwo) ;
  if(lowerLimit == upperLimit) return upperLimit;
  return lowerLimit+"-"+upperLimit;
}
function fp_EncodeUspRange(xid,list_values,conf_values)
{
  for(var i=0;i<conf_values.length;i++)
  {
    if(conf_values[i] == fp_nullValue) continue;

    for(var j=0;j<conf_values.length;j++)
    {
      if(conf_values[j] == fp_nullValue) continue;

      if(fp_getDiff(list_values,parseAndGetUpperLimit(conf_values[i]),parseAndGetLowerLimit(conf_values[j])) == -1)
      {
        conf_values[i] = fp_prepareRange(conf_values[i],conf_values[j]);
        conf_values[j] = fp_nullValue;
      }
    }
  }

  var tmpconf_values = fp_getNonNullValues(conf_values);
  var tmpStr="";
  for(var index=0;index<tmpconf_values.length;index++)
      tmpStr = tmpStr + tmpconf_values[index] + ",";

  if(tmpStr.indexOf(",") == tmpStr.length -1)  {
     tmpStr = tmpStr.substring(0,tmpStr.length-1);
  }

  return tmpStr;
}
function fp_DecodeUspRange(xid,list_values,rangeStr){
  var range = new Array();

  if(rangeStr.indexOf(",") != -1)
  {
    range=rangeStr.split(",");
  }
  else
  {
    range[0]  = rangeStr;
  }
  var length = range.length;

  for(var i=0;i<length;i++){
   var upperLimit = fp_parseAndGetUpperLimit(range[i]);
    var lowerLimit = fp_parseAndGetLowerLimit(range[i]);

    if(upperLimit == lowerLimit) continue;

    var j=lowerLimit;
    do
    {
      if(!fp_contains(range,j))
      {
        range[range.length] = j;
      }
      j=fp_getIncr(list_values,j);
    }while(fp_getDiff(list_values,j,upperLimit) <= 0) ;

    range[i] = fp_nullValue;
  }

  return fp_getNonNullValues(range);

}

function fp_contains(conf_values,element)
{
  for(var index=0;index<conf_values.length;index++)
  {
    if(conf_values[index] == element) return true;
  }
  return false;
}


function macFilter_SubmitStart(submitFlag){
}

function macFilter_SubmitEnd(submitFlag){
}
function macFilter_LoadEnd(arg){
}

function fp_getElemValueList(xid){
   var element = getInputElementByXid(xid);

   if(element == null || element.nextSibling == null || element.nextSibling.options == null) return new Array();

   var options = element.nextSibling.options;
   var list_values  = new Array();
   for(var index=0;index<options.length;index++){
       list_values[list_values.length]=options[index].value;
   }

  return list_values;
}

function macFilter_LoadStart(arg){
    xuiTypeRegister("uspRange_t", fp_EncodeUspRange, fp_DecodeUspRange);
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
function fp_getNonNullValues(array)
{
  var result_array= new Array();
  for(var index=0;index<array.length;index++)
  {
    if(array[index] != nullValue)
    {
      result_array[result_array.length]  = array[index];
    }
  }
  return result_array;
}
