
var doWeSupportCascading = false;

function tokenize(inputString,ignoreEmptyTokens) 
{
  var input       = inputString;
  var separator   = "|";
  var trim        = " ";
  
  var i;
    
  var array = input.split(separator);
    
  if(trim) 
  {
    for(i=0; i<array.length; i++) 
    {
      while(array[i].slice(0, trim.length) == trim) 
      {
        array[i] = array[i].slice(trim.length);
      }
      while(array[i].slice(array[i].length-trim.length) == trim) 
      {
        array[i] = array[i].slice(0, array[i].length-trim.length);
      }             
    }
  }
    
  var tmpArray = new Array();
  if(ignoreEmptyTokens) 
  {
    for(i=0; i<array.length; i++) 
    {
      if(array[i] != "") 
      {
        tmpArray.push(array[i]);
      } 
      else 
      {
        tmpArray.push(0);
      }
    }
  } 
  else 
  {
    tmpArray = array;
  }
  return tmpArray;
}

// the passed elements contains generated dom elements
// which includes td/tr/input/th/table
// this function  will return the dhtml dom elements
function getExactTargetsArray(elements_array)
{
  if(elements_array == null) return null;
    
  var targets_array = new Array();
    
  for(var index=0; index<elements_array.length;index++)
  {
    var ent = elements_array[index];
    
    
    //avoid duplicate elements
    /*if(!contains(targets_array,ent)){
       targets_array[targets_array.length] = ent;
    } */
    //contains check not needed as elements_array will have unique elements
    
    targets_array[targets_array.length] = ent;
        
    //for hidden elements, get the next sibling         
    if(ent.type != null && ent.type == "hidden" )
    {
       var node = ent.nextSibling;
       
       while(node != null){
          if(node.type != "hidden" && 
            !contains(targets_array,node) &&  node.tagName != null ){
            targets_array[targets_array.length] = node;
          }   
          if(node.nextSibling == null || node.nextSibling.type == "hidden")
             break;
           
          node = node.nextSibling;
       }
    }   
    //for TR/TD elements, get the child elements
    else  
    {            
       //childNodes = $A(ent.getElementsByTagName("input"));
       childNodes = MapElements.GetChildElementsByTagNames("INPUT",ent);
       for(var i=0;i<childNodes.length;i++){
         if(!contains(targets_array,childNodes[i]))  
           targets_array[targets_array.length] = childNodes[i];   
        
       }
      
    }                
 
}
    
  return targets_array;
}

function isDigit(ch)
{
  if(ch == '0' ||  ch == '1' ||  ch == '2' ||  ch == '3' ||
    ch == '4' ||  ch == '5' ||  ch == '6' ||  ch == '7' ||
    ch == '8' ||  ch == '9') 
  {
    return true;
  }
    
  return false;
}

function hasPrefix(text)
{
  var ch1=text.charAt(0);
  var ch2=text.charAt(1);
    
  if(isDigit(ch1) && ch2 == '.')
  {
    return true;
  }
    
  return false;
}

function trim(text)
{    
  while(text.charAt(0) == ' ')
  {
    text = text.substring(1,text.length);
  }
    
  while(text.charAt(text.legth-1) == ' ' )
  {
    text = text.substring(0,text.length-1);
  }    
  
  return text;
}

//extract the prefix from the given element name
//the name can be of the form 1.2.3.xobj_name_1_2_1 for the elements of a table
//extract and return 1.2.3
//return null if name does not have a prefix

function name2Prefix(name)
{    
  var prefix ="";
  var index=0;
  var text = new String(name);
  text = trim(text);
    
    
  if(hasPrefix(text))
  {
    do
    {
      prefix= prefix + text.charAt(index++);            
    }while(isDigit(text.charAt(index)) ||  text.charAt(index) == '.');
       
    return prefix;           
  }
        
  return null;
        
}

// element name is used to see if this is from repeat
// in which case we get some thing like 1.2.3.ObjName
// This function will search for all the dom elements 
// with the id equal to xid
function getActionElements(prefix, xid)
{
  var elements_array = MapElements.GetChildElementsByXid(xid);
  var result_array = elements_array;
    
    
  if(elements_array == null || elements_array.length == 0)
  {       
    return new Array();
  }
  
  //if the element has the prefix, prepare an array with  
  //the elements having the same prefix
  
  if(prefix != null)
  {        
    for(var index=0;index<elements_array.length; index++)  
    {
      if(name2Prefix(elements_array[index].name) == prefix)
      {
        result_array = new Array();
        //result_array[0]  = elements_array[index];
        var ent = elements_array[index];
        result_array[0] = ent;
        if(ent.type != null && ent.type == "hidden" )
        {
             var node = ent.nextSibling;                
             while(node != null){
                 if(node.type != "hidden" && node.tagName != null ){
                    result_array[result_array.length] = node;
                 }   
                 if(node.nextSibling == null || node.nextSibling.type == "hidden")
                    break;                
                 node = node.nextSibling;
             }
        }   
        break;
      }
    }
        
  }
  //
  //collect the dhtml dom elements along with the html elements
  //collected into result_array
  //
  //result_array = getExactTargetsArray(result_array);
    
  return result_array;
    
}



function xuiGetInputElements(prefix , Ids){
  var resultArray = new Array();
  if(Ids == "") 
  {
    return resultArray;
  }
   
  var xids = tokenize(Ids,true);
  
   
  for ( var i = 0; i < xids.length; i++ ) 
  {   
     var elements_array = MapElements.xuiGetElementsWithPrefix(prefix,xids[i]);
     for(var k=0; k< elements_array.length; k++)
     {                                
        var obj = elements_array[k];
        if(obj == null || obj.getAttribute('xid') == null || obj.getAttribute('type') != "hidden" ) continue;
        resultArray.push(obj);
     }
  }  
  
  return resultArray;
  
  
  
  
}

function xuiCopyVal(prefix , xcopy, exec)
{
  
  if(xcopy == null || xcopy.length != 2)
  {
     return;
  }
  
  if(xcopy[0] == "" || xcopy[1] == "") 
  {
    return;
  }
   
  var srcXids = tokenize(xcopy[0],true);
  var dstXids = tokenize(xcopy[1],true);
  
  if(srcXids.length != dstXids.length) return;
  
  for ( var i = 0; i < srcXids.length; i++ ) 
  {   
       var entVal = xuiGetVal(prefix,srcXids[i]);
       if(entVal == null) continue;
       xuiSetVal(prefix,dstXids[i],entVal,exec);
     
  }
  
}


function xuiGetVal(prefix,xid)
{
    var elements_array = getActionElements(prefix,xid);            
    var ent = null;
  
    for(var k=0; k< elements_array.length; k++)
    {                                
       var obj = elements_array[k];
       if(obj == null || obj.getAttribute('xid') == null || obj.getAttribute('type') != "hidden" ) continue;
       ent = obj; break;
    }   
     
    if(ent == null) return;
    
    var xc = xeData["xc_" + xid];
    var xe = xeData["xe_" + xid];    
    var xtn = (xeData["xtn_" + xid] == null)? new Array() : xeData["xtn_" + xid];
    var xDType = xeData["xd_" + xid]; 
    var j, node;
    var eleName ;
    var entVal = null;
    
    switch(xc) 
    {
      case valmap.label:
        if(ent.nextSibling != null)
        {
          entVal = ent.nextSibling.nodeValue;
        }
        break;
          
      case valmap.textbox:
      case valmap.textarea:
      case valmap.passwdbox:
      
        entVal = ent.nextSibling.value;                
        break;
               
      case valmap.radio:            
      case valmap.radio_left:
      
        if(xDType == "refresh"){           
           var ev =   getAllRefProfiles(xid);
         }
         else{             
             var ev = getAllWebEnums(xid) ;
         }   
        var node = ent.nextSibling;
        
        while(node != null){
          if(node.checked == true) 
          {
            entVal = node.value;
            break;
          }
          node = node.nextSibling;
        }
        break;
            
      case valmap.radio_none:
      
        
        if(xDType == "refresh"){           
           var ev =   getAllRefProfiles(xid);
         }
         else{             
             var ev = getAllWebEnums(xid) ;
         }   
        /* need to add the logic to uncheck if the a radio is selected twice */        
        var node = ent.nextSibling;
        
        while(node != null){
          if(node.checked == true) 
          {
            ent.value = node.value;
            break;
          }
          node = node.nextSibling;
        }
        break;
                
      case valmap.combo:
        
            var selected_index = ent.nextSibling.selectedIndex;
            if(xDType == "refresh"){           
               var ev =   getAllRefProfiles(xid);
            }
            else{             
               var ev = getAllWebEnums(xid) ;
            }   
            var item_count     = ev == null?0:ev.length;
            var ea_indices     = xtn;
            
            if(ev != null && selected_index <= ev.length-1)
            {                
               entVal = ev[selected_index];
            }
            break;
            
      case valmap.combo_multiselect:
        
         if(ent.nextSibling.xvalue != undefined) 
         {
            entVal = ent.nextSibling.xvalue;
         }
          
        break;
            
      case valmap.checkbox:
        
        if(xDType == "refresh"){           
           var ev =   getAllRefProfiles(xid);
         }
         else{             
             var ev = getAllWebEnums(xid) ;
         }   
        if(ent.nextSibling.checked == true) 
        {
          entVal = ev[0];
        } 
        else 
        {
          entVal = ev[1];
        }
        break;
            
      case valmap.checkboxNeg:
        
        if(xDType == "refresh"){           
           var ev =   getAllRefProfiles(xid);
         }
         else{             
             var ev = getAllWebEnums(xid) ;
         }   
        if(ent.nextSibling.checked == true) 
        {
          entVal = ev[1];
        } 
        else 
        {
          entVal = ev[0];
        }
        break;     
            
      case valmap.button:
      case valmap.url:
      case valmap.file:
      case valmap.hidden:
        entVal = ent.value;  
        break;
                    
      default:
        alert("xuiGetVal: unknown control : '" + xc + "' for element '" + ent.name + "'");
        break;
    }
        
       
  return entVal;
  
}
   

function xuiSetVal(prefix , Ids, Args, exec)
{
  if(Ids == "") 
  {
    return;
  }
   
  var xids = tokenize(Ids,true);
  var values = tokenize(Args,false);
   
  for ( var i = 0; i < xids.length; i++ ) 
  {   
    var elements_array = getActionElements(prefix,xids[i]);            
    for(var k=0; k< elements_array.length; k++)
    {                                
                
      var obj = elements_array[k];
      if(obj == null || obj.getAttribute('xid') == null || obj.getAttribute('type') != "hidden" ) continue;
      
      var xid = obj.getAttribute('xid'); //xid and xids[i] may not be the same as in the case xid=1_2_1 and xids[i] = 1_2
      
      var xc = xeData["xc_" + xid];                
                
      switch(xc) 
      {
        case valmap.label:
          obj.value = values[i];
          obj.setAttribute("copyVal" , values[i]);
          break;

        case valmap.textbox:
        case valmap.textarea:
        case valmap.passwdbox:
          obj.nextSibling.value = values[i];
          break;
        case valmap.hidden:
          obj.value = values[i];
          break;
        case valmap.radio:
        case valmap.radio_none:
        case valmap.radio_left:
               
          var node = obj.nextSibling;
          
          while(node != null){
          
             if(node.value == values[i])
             {
               node.checked =  true; 
               xuiSwitchPageActions(xid, obj, exec);
               break;
             }
             node = node.nextSibling;
          }
          
          break;
                       
        case valmap.checkbox:
                  
          ev = getAllWebEnums(xid);
          if(ev[0] == values[i])
          {
            obj.nextSibling.checked = true;
            obj.nextSibling.value=values[i]; 
            xuiSwitchPageActions(xid, obj, exec);
          }   
          else if(ev[1] == values[i])
          {
            obj.nextSibling.checked =  false; 
            obj.nextSibling.value=values[i];
            xuiSwitchPageActions(xid, obj, exec);
          }    
          break;
                       
        case valmap.checkboxNeg:
                    
          ev = getAllWebEnums(xid);
          if(ev[1] == values[i])
          {
            obj.nextSibling.checked = true;
            obj.nextSibling.value=values[i]; 
            xuiSwitchPageActions(xid, obj, exec);
          }   
          else if(ev[0] == values[i])
          {
            obj.nextSibling.checked =  false; 
            obj.nextSibling.value=values[i];
            xuiSwitchPageActions(xid, obj, exec);
          }    
          break;
                       
        case valmap.combo:
          ev = getAllWebEnums(xid); 
          for(var index=0; index<ev.length; index++)
          {
            if(ev[index] == values[i])    
            {
              obj.nextSibling.selectedIndex = index;
              xuiSwitchPageActions(xid, obj, exec);
            }
          }
          break;
        case valmap.combo_multiselect:
          var tmp_array = new Array();
          ev = getAllWebEnums(xid); 
          
          for(index=0; index<ev.length; index++)
          {
            if(ev[index] == values[i])    
            {
              obj.nextSibling.options[index].selected=true;
              tmp_array[tmp_array.length]  = obj.nextSibling.options[index].value; 
            }
            else
            {
              obj.nextSibling.options[index].selected=false; 
            }
          }
                  
          obj.nextSibling.xvalue = xuiTypeEncode(xid,tmp_array);         
          xuiSwitchPageActions(xid, obj, exec);
          break;   
                      
        default:
          alert("This is unknown control : " + xc + " for element " + obj.name);
          break;        
      }                
    }            
  }                          
}

//
//perform show/hide/disable/enable for the elements
//corresponding to Ids passed in str in the format 1_2|1_3|1_4
//If prefix is specified elements will be filtered by prefix matching.
//
function xuiShed(which, prefix , element_id , str ) 
{    
  if(str == "") 
  {
    return;
  }
   
  var xids = tokenize(str,true);
  var enabled_xids=new Array();
  var disabled_xids=new Array();
  
  var req_tagNames = "INPUT,SELECT,TEXTAREA";
   
  // for all the ids from xml shed
  for ( var i = 0; i < xids.length; i++ ) 
  {                                  
    var elements_array = getActionElements(prefix,xids[i]);            
    var perm = 0;
    var xp;
    for(var k=0; k< elements_array.length; k++)
    {                                
      var obj = elements_array[k];
      
      if(obj == null) continue;
      
      var xp = xeData["xp_" + xids[i]];
      
      var Id =   ( obj.getAttribute('id') == null || obj.getAttribute('id') == "")? 
                 ((obj.getAttribute('xid') == null)? null : 
                   obj.getAttribute('xid')):
                   obj.getAttribute('id');   
               
      var xc = xeData["xc_" + Id];
      
      if (obj.getAttribute('type') != "hidden") {
        perm = (xp == valmap.read_only) ? 1 : 0
      }
                
      switch(which) 
      {
        case 0: //show
                
          //do not show the hidden elements      
          if(xc == valmap.hidden){
             break;
          }
          //do not show TR until it's children are shown
          if(obj.tagName == "TR" ){
             break; 
          }
          var tmp = obj;
          while(tmp != null )
          {
            if(tmp.style != null)
               tmp.style.display = "";
            //loop through backwards till parent TABLE is reached. 
            if(tmp.tagName == 'TABLE') 
               break;
             tmp = tmp.parentNode;
          }
                                       
          break;
                    
        case 1: //hide
                    
          if(obj.style != null)
          {
            obj.style.display = "none";
          }
                    
          break;
                    
        case 2: //disable
        
        if(obj.nodeType == 1 && req_tagNames.indexOf(obj.tagName) != -1 && obj.disabled == false){
			xuiPreserveValue(obj, 'disabled', obj.disabled);
           obj.disabled= true;
        }
      /*curr = elements_array[k];
      if (k)
        prev = elements_array[k-1];
      else
        prev = null;

      if(!((curr.getAttribute('xid') != null && curr.getAttribute('type') == "hidden" ) ||
         (prev && (prev.getAttribute('xid') != null || prev.getAttribute('type') == "hidden")))) 
      {
         curr.disabled = false;
         continue;
      }

        if (obj.disabled == true)continue;

              obj.disabled = true;

          

          //avoid forwarding the disable event to same element
          //in case the actions of an element refers to the element itself
          //                    
          if(!contains( disabled_xids,xids[i] ) && element_id != xids[i])
          {
            disabled_xids[disabled_xids.length] = xids[i];
          }*/
                   
          break;
                    
        case 3: //enable
       //perm flag checks for read-only 
        if(obj.nodeType == 1 && req_tagNames.indexOf(obj.tagName) != -1 && obj.disabled == true && perm == 0){
			xuiPreserveValue(obj, 'disabled', obj.disabled);
           obj.disabled= false;
        }
         
    /*  curr = elements_array[k];
      if (k)
        prev = elements_array[k-1];
      else
        prev = null;

      if(!((curr.getAttribute('xid') != null && curr.getAttribute('type') == "hidden" ) ||
         (prev && (prev.getAttribute('xid') != null || prev.getAttribute('type') == "hidden"))))
      {
         curr.disabled = false;
         continue;
      }
      
         

          if(obj.disabled == false) continue;
          obj.disabled = false;
          
          
          //avoid forwarding the enable event to same element
          //in case the actions of an element refers to the element itself
          //                     
          if( !contains( enabled_xids,xids[i] ) && element_id != xids[i])
          {
            enabled_xids[enabled_xids.length] = xids[i];
          }*/
                   
          break;
                    
        default:
          break;
      }
    }
             
  }  
  if(doWeSupportCascading)
  {
    forwardDisableEvent(disabled_xids,prefix);
    forwardEnableEvent(enabled_xids,prefix);        
  }   
}

//disable all the dependent elements for the disabled elements
function forwardDisableEvent(xids,prefix)
{
  for(var i=0;i<xids.length;i++)
  {    
    var targets = new Array();
    collectDependentIds(targets, xids[i]);
    for(var j=0;j<targets.length;j++)
    {
      xuiShed(2,prefix,xids[i],targets[j]);
    }
  }    
}

//
//perform the actions(show/hide/disable/enable) associated with the elements
//that are depending upon the enabled elements
//
function forwardEnableEvent(enabled_xids,prefix)
{
  var ev = null;
  
  for(var i=0;i<enabled_xids.length;i++)  
  {                    
    var xid            = enabled_xids[i];
    var elements_array = MapElements.GetChildElementsByXid(xid);
    if(elements_array == null) continue;
                       
    elements_array = getExactTargetsArray(elements_array);
            
    //execute the actions of dependent elements 
    for(var index=0;index<elements_array.length;index++)
    {               
      var element        =  elements_array[index];
      var type           =  element.type;//applicable for radio/checkbox
      var selectedIndex  =  element.selectedIndex; //aplicable for combo
      var xc             =  xeData["xc_" + xid]  ;
      
      //handle checkbox          
      if(xc == valmap.checkbox && type=="checkbox")
      {                    
        ev = getAllWebEnums(xid);
        xuiProcessCheckBoxActions(element, xid, element.name,ev[0],ev[1]);
      }   
      //handle checkboxNeg
      else if(xc == valmap.checkboxNeg && type=="checkbox")
      {                    
        ev = getAllWebEnums(xid);
        xuiProcessCheckBoxActions(element, xid, element.name,ev[1],ev[0]);
      }   
      //handle radio
      else if(type=="radio" && (xc==valmap.radio || xc==valmap.radioLeft || xc==valmap.radioNone))
      {
        xuiProcessJSActions(xid,element.name,element.value);
      }
      //handle combo
      else if(selectedIndex != -1 && xc==valmap.combo)
      {
        xuiProcessJSActions(xid,element.name,element.value);
      }  
    }
  }         
}

//
//All Shed/SetVal routines assumes that xa is in the following format:
//  xa[0] = value
//  xa[1] = position
//  xa[2] = invoke
//  xa[3] = show
//  xa[4] = hide
//  xa[5] = disable
//  xa[6] = enable
//  xa[7] = setval_ids
//  xa[8] = setval_args
//
//and the same structure gets repeated for all the possible options of an element
//

//dependent Ids of an element are the Ids specified in element's
//show/hide/disable/enable/setVal actions
//
function collectDependentIds(xids,xid)
{      
  var xa = xeData["xa_" + xid];
  if(xa == null)
  {  
    return 0;           
  }
    
  for(var j=0; j<xa.length; j+=7) 
  {        
    //disable Ids
    if(!contains(xids,xa[j+3]))
    {
      xids[xids.length] = xa[j+3];           
    }        
    //enable Ids
    if(!contains(xids,xa[j+4]))
    {
      xids[xids.length] = xa[j+4];           
    }            
  }
   
  return 0;
}

function xuiDelegateJSActions(ent, xa, name,xid , off, exec, target,JSparam) 
{
  ++off; /* for position */ 
  ++off; /* for invoke */
  xuiShed(0, name2Prefix(name),xid, xa[++off]); //show
  xuiShed(1, name2Prefix(name),xid, xa[++off]); //hide
  xuiShed(2, name2Prefix(name),xid, xa[++off]); //disable
  xuiShed(3, name2Prefix(name),xid, xa[++off]); //enable
  xuiSetVal(name2Prefix(name), xa[++off], xa[++off], exec); //setval
  xuiCopyVal(name2Prefix(name),xeData["xcopy_" + xid + "_" + JSparam],exec); //copyVal
  xuiJSFunc(xid,JSparam);// js-function call in action
  xuiPageRedirect(ent,xid,target, exec); // page redirection 
  return 0;
}

function xuiJSFunc(xid,JSparam)
{

 
           if (xeData["js_" + xid + "_" + JSparam] != null){              
               xuiRunJSFunc(xeData["js_" + xid + "_" + JSparam][0],xeData["js_" + xid + "_" + JSparam][1]);//call user-defined function
           }

}
//delegate the actions associated with a button
//button needs special handling as we don't check for button's value
//and perform it's actions whenever this function is called
//
function xuiProcessButtonActions(ent,xid)
{    
  xuiInitPreserved();
  xuiShed(3, null, xid, xid); //enable this button object info
  
  var xa = xeData["xa_" + xid];
  if(xa == null)
  {  
    return xuiPageRedirect(ent,xid,xeData["xrd_" + xid], valmap.onClick);
  }
   
  //For button Action invokes always onCLick , for now assuming onCLick and onChange to be same 
  for(var j=9; j<xa.length; j+=9) 
  {      
    xuiDelegateJSActions(ent,xa, "" , xid , j, valmap.onClick, xeData["xrd_" + xid],"button");             
  }
  return 0;            
}

function preJSAction(){
    return true;
}
function postJSAction(){
   return true; 
}

function xuiProcessJSActions(ent,xid, name, value, exec) 
{    
  var invoke = "";  
  var defFlag = true;
  var xa = xeData["xa_" + xid];
  
  if(xa == null)
  {  
    return 0;        
  }
   
  for(var j=18; j<xa.length; j+=9) 
  {
    invoke = xa[j+2];
    if(xuiCheckActionPropertyToExec(invoke, exec))
    {                
           //
      //evaluate the regular expression  and compare the 
      //actual value against regular expression
      //
          
      var result = null;
      var regex = new RegExp(xa[j]);
      
      if(regex == null)     {
         alert("Invalid regular expression  " + xa[j]);
         return;
      }
      
      result = regex.exec(value) ;
        
      //value matched with the regular expression pattern
      if(result != null && RegExp.leftContext.length ==0 && RegExp.rightContext.length ==0)
      {
            xuiDelegateJSActions(ent,xa, name, xid , j, exec,xeData["xrd_"+ xid + "_" + xa[j]],value);                          
            defFlag = false;
            break;
      }    
    }    
  }  
  if(defFlag == true)
  {    
     xuiExecDefaultJsActions(ent,xa, name, xid, exec,0);     
  }
  //Mandate Action
  xuiExecMandateJSActions(ent,xa,name,xid, exec, 9);
  
  
} 
function xuiExecDefaultJsActions(ent,xa, name, xid, exec, index)
{
   var invoke = xa[index+2];
   if(xuiCheckActionPropertyToExec(invoke, exec))
   {
      xuiDelegateJSActions(ent,xa, name, xid , 0, exec,xeData["xrd_" + xid ],"default" );
   }
   return ;
}
function xuiExecMandateJSActions(ent,xa,name,xid, exec, index)
{
   var invoke = xa[index+2];
   if(xuiCheckActionPropertyToExec(invoke, exec))
   {
       xuiDelegateJSActions(ent,xa, name, xid , 9, exec, xeData["xrd_" + xid ],"mandate" );
       
   }
   return ;
}
function xuiCheckActionPropertyToExec(prop, exec)
{  
    if(exec == valmap.onLoad)
    {
        if(prop == "onLoad" || prop == "both"){
            return 1;
        }
        else{
            return 0;
        }
    }
    else if(exec == valmap.onChange)
    {
        if(prop == "onChange" || prop == "both"){
            return 1;
        }
        else{
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
// function  to handle onchange action
/*function xuiProcessJSOnChangeActions(xid,name,value)
{
    var xa = xeData["xa_"+xid];
    if(xa == null)
    {
        return 0;
    }       
    if(xa[7] == value)
    {    
        return xuiDelegateJSActions(xa,name,xid,j, exec);
    }       
}*/
//
//function to handle combo-multiselect refresh
//prepares the range string in the format "0/1,0/3-0/5,0/8"
//and assign it to element's value
//
function xuiProcessMultiSelectRefresh(ent,doRef,selected_index,xid,element)
{ 
  //get the flag for JS Actions
  if(!preJSAction())   return;
    
  var options = element.options;
  var values = new Array();
  
  //process JS actions
  xuiProcessMultiSelectActions(ent,xid, element.name, options, valmap.onChange);
  
  for(var index=0;index<options.length;index++)
  {
    if(options[index].selected == true)
    {
      values[values.length] = options[index].value;     
     
    }
  }
  
  var tmp = xuiTypeEncode(xid, values);
  //alert("xuiProcessMultiSelectRefresh: tmp = " + tmp);
  //element.xvalue = new String(tmp); // we can't use element.value
  element.xvalue = tmp; // we can't use element.value
  //alert("xuiProcessMultiSelectRefresh: element.xvalue = " + element.xvalue);
  var xtn = (xeData["xtn_" + xid] == null)? new Array() : xeData["xtn_" + xid];
  
  if(doRef == true && !contains(xtn,selected_index))
  {  
    formSubmit(xui_operation_reload,null,null);
  }
  
}

//
//function to handle the combo-multiselect refresh
//will execute the actions of all the options selected
//
function xuiProcessMultiSelectActions(ent,xid,name, options, exec)
{
  for(var index=0;index<options.length;index++)
  {
    if(options[index].selected == true)
    {
      xuiProcessJSActions(ent,xid, name, options[index].value, exec);
    }
  }     
}


function xuiPageRedirect(ent, xid, redirectionData, invoke){
  
   if(redirectionData == null || redirectionData.length == 0)
     return;
   
   var target = redirectionData[0];
  
   if((invoke == valmap.onClick || invoke == valmap.onChange) && target != "#" && target != null){
      var redirectIds = redirectionData[1]   ;
      var elements = xuiGetInputElements(ent.parentNode.getAttribute('p'), redirectIds);
      //mark the elements to be redirected to , with the attribute redirectTo
      for(var index=0;index<elements.length;index++){
          elements[index].setAttribute('redirectTo', true);
      }    
      var submitFlag = (xeData["xt_" + xid] == null)? xui_operation_redirect : xeData["xt_" + xid]; 
      formSubmit(submitFlag,xid,target);
   }
   
}


//function to handle the check/uncheck for radio buttons in a radio-group
function xuiProcessOnLoadRadioGroup () 
{
  
   var rGroup = xeData["rGroup"];
   
   if(rGroup == null){
      return;
   }
   //
   //generated rGroup format Ex:  ["1_1_1,1_2_1,1_3_2,1_3_3" , [$$1_1_1,1_2_1], [$$1_3_2,1_3_3]]
   //where 1_1_1,1_2_1 are the ids of elements belonging to a radio group 
   //and 1_3_2,1_3_3 belonging to adifferent radio group
   //$$1_1_1 represents that  1_1_1 is the default radio button
   //
   
   
   for(var i=1; i<rGroup.length;i++)
   {
       var tmpGroup = rGroup[i];
       var defaultRadio_xid  = null;
       var checkCount =0;
       var firstRadio_xid = null; 
        
       for(var j=0;j<tmpGroup.length;j++)
       {
           
            var radio_xid = (tmpGroup[j].indexOf('$$') == -1)? tmpGroup[j]:
                             tmpGroup[j].substr(2,tmpGroup[j].length);
        
            var rValue = getRadioValue(radio_xid);
            var ent    = MapElements.GetNonTabularHiddenInputElementByXid(radio_xid);
            
            if(ent == null){
              continue;
            }
            
            if(ent.value == rValue && checkCount == 0) 
            {
              xeData["check_" + radio_xid] = "CHECKED";
              checkCount++;
            } 
            else 
            {
              xeData["check_" + radio_xid] = "";
            }
            //store default radio
            if(tmpGroup[j].indexOf('$$')  != -1)    
            {
               defaultRadio_xid = radio_xid;
            } 
            //store first radio
            if(j==0){
               firstRadio_xid = radio_xid;
            }
       
      }  
    
      if(checkCount == 0 && defaultRadio_xid != null)
      {
           xeData["check_" + defaultRadio_xid] = "CHECKED";
      }
     
          
 }     
            
        
    
}


//function to handle the onclick event for radion in a radio group
function xuiProcessOnChangeRadioGroup (doRef,ent, selected_index, xid, name, exec) 
{
  
   var rGroup = xeData["rGroup"];
   
   //
   //generated rGroup format Ex:  ["1_1_1,1_2_1,1_3_2,1_3_3" , [$$1_1_1,1_2_1], [$$1_3_2,1_3_3]]
   //where 1_1_1,1_2_1 are the ids of elements belonging to a radio group 
   //and 1_3_2,1_3_3 belonging to adifferent radio group
   //$$1_1_1 represents that  1_1_1 is the default radio button
   //
   
   
   var foundGroup= false;
   var radioElements = new Array();
   var defaultRadio  = null;  
   
   for(var i=1; i<rGroup.length;i++){
       var tmpGroup = rGroup[i];
       radioElements = new Array();
       defaultRadio  = null;
       
       for(var j=0;j<tmpGroup.length;j++){
           
            var radio_xid = (tmpGroup[j].indexOf('$$') == -1)? tmpGroup[j]:
                             tmpGroup[j].substr(2,tmpGroup[j].length);
           
            var tmp_array = MapElements.GetRadioElementsByXid(radio_xid); 
            
            for(var index=0;index<tmp_array.length;index++)
               radioElements.push(tmp_array[index]);
            
            if(radio_xid == xid)   
               foundGroup = true;
            
            if(tmpGroup[j].indexOf('$$')  != -1)    
               defaultRadio = tmp_array[0];
            
            
       }
       if(foundGroup) break;
   }
  
   //on pageLoad, check the default radio element if none of the radio elements is checked
   if(exec == valmap.onLoad){
      var checkCount=0;
      for(var index=0;index<radioElements.length;index++){
         if(radioElements[index].checked == true) checkCount++;
         //if more than one radio checked, uncheck them except the first one
         if(checkCount > 1) radioElements[index].checked = false;
         
      }
      //
      //none of the radio elemnts checked
      //check the default radio in the group
      //
      if(checkCount == 0 && defaultRadio != null) {
         defaultRadio.checked= true;
      }
      
      
   }
   //onRadio click event
   else{
       for(var index=0;index<radioElements.length;index++){
          //check for current radio button, on which the event has occurred
          if(radioElements[index] == ent) continue;
          //uncheck the remaining radio buttons in the group
          radioElements[index].checked= false;
       }
       xuiProcessOnChangeRefresh (ent,doRef, selected_index, xid, name) ;
     
   }
          
    
}

// special function to support the on change refresh
// rest of it same as xuiProcessJSActions
function xuiProcessOnChangeRefresh (ent,doRef,selected_index, xid, name) 
{
  //get the flag for JS Actions
  if(!preJSAction())   return;
  
  //set the elemtn value to selected option
    var xe  = xeData["xe_" + xid]; 
    var xrv = xeData["xrv_" + xid];
    /* reading whether this is an element meant for refresh */
    var xRef = xeData["xd_" + xid];
    if(xRef == "refresh"){
        var ev = getAllRefProfiles(xid);        
    }else{
        var ev  = getAllWebEnums(xid);
    }   
    var xtn =(xeData["xtn_" + xid] == null)? new Array() : xeData["xtn_" + xid];
    
    var val = (xrv == null)? ev[selected_index] : xrv;
    //process JS Actions
    xuiProcessJSActions(ent,xid, name, val, valmap.onChange);   
    
    //skip the page refresh if the selected item is an Extended Actions Item
    //Whose index will start after all the possible values of the element
    //skip count is the count of extended action items in this element.
    if(doRef == true && !contains(xtn,selected_index)) 
    {
      formSubmit(xui_operation_reload,null,null);
    }
}

// special function for checkbox
// we need to use the value from enum
// based on if the control is checked or not
// rest of it same as xuiProcessJSActions
function xuiProcessCheckBoxActions(obj, xid, name,val1, val2) 
{  
  //get the flag for JS Actions
  if(!preJSAction())   return;
    
  if(obj.checked == null)   return;
  var val = obj.checked? val1 : val2;
  xuiProcessJSActions(obj,xid,name, val, valmap.onChange);
}

// special function for radio-none
// we need to get the object and select/unselect 
// depending on the current state
// rest of the funcionality is same as xuiProcessOnChangeRefresh
function xuiProcessShedRefreshRadioNone (obj, doRef, selected_index, xid, name,value) 
{
  //get the flag for JS Actions
  if(!preJSAction())   return;
    
  var newid = xid+"_prevValue";
  var prevValue = null;
  var hiddenEle = document.getElementById(newid);
  
  if(hiddenEle != null)
  {
    prevValue = hiddenEle.value;
  }
   
  if (prevValue == value) 
  {
    obj.checked = false;
    hiddenEle.value = null;
  }
  else if(obj.checked)
  {
    hiddenEle.value = value;
  }
  xuiProcessOnChangeRefresh(obj,doRef, selected_index, xid, name);
}

var xuiPreservedHash = undefined;
function xuiPreserveValue(obj, which, value)
{
  if(typeof(xuiPreservedHash) == "undefined") return;
  var index = xuiPreservedHash.length;
  xuiPreservedHash[index] = new Object();
  xuiPreservedHash[index].obj = obj;  
  xuiPreservedHash[index].which = which;
  xuiPreservedHash[index].value = value;
}

function xuiRevertPreserved()
{
  if(typeof(xuiPreservedHash) == "undefined") return;
	for(var index = 0; index < xuiPreservedHash.length; index++) {
		var obj = xuiPreservedHash[index].obj;
		var which = xuiPreservedHash[index].which;
		var value = xuiPreservedHash[index].value;
		if(which == 'disabled') obj.disabled = value;
		else if(which == 'value') obj.value = value;
	}  
}

function xuiClearPreserved()
{
  xuiPreservedHash = undefined;
}

function xuiInitPreserved()
{
  xuiPreservedHash = new Array();
}