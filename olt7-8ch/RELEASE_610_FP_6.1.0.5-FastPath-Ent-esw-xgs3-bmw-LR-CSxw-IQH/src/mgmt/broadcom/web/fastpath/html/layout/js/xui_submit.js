var ENTER_KEY_VALUE = 13;

function xuiValidateElement(xid, value)
{
  var xd = xeData["xd_" + xid];  
  var xdt = null;
  var dt = null;
  if(xd == "user")
  {
       xdt = xeData["xdt_"+xid];
  } 
  if(xd == null) return 1;
  if(xdt!= null)
  {
      dt = xdt;
  }
  else
  {
      dt = xd;
  }
  
  var retval = jScriptValidate(dt,value);
  if(retval != null)
  {
    return xuiDynamicError(xid,value,retval);
  }
    
  //  we need to perform UI and element level validations
  retval = jScriptValidateUiAndElementLevel(xid,value);
  if(retval != null)
  {
    return xuiDynamicError(xid, value, retval);
  }
  // perform min-max validations
   retval = jScriptValidateMinMax(xd, value, xid);
    
    if (retval != null)
    {
        //alert(retval);
        return xuiDynamicError(xid, value, retval);
    }  
  return true;
}
  
function xuiPageSubmitRender(validate,bid,pageRedirect)
{
  var f = MapElements.Get('INPUT');
  var value  = true;
  for(var i=0; i<f.length; i++)
  {
    var ent = f[i];
    var xid = ent.getAttribute('xid'); if(xid == null) continue;
    var xc = xeData["xc_" + xid];
    var xe = xeData["xe_" + xid];    
    var xtn = (xeData["xtn_" + xid] == null)? new Array() : xeData["xtn_" + xid];
    var j, node;
    var diabled = false;        
    var xValue = ent.getAttribute('xvalue');
    var eleName ;
    var ignoreVals = xeData["xopt_" + xid];
    /* value os xDtype will be "refresh if the element is of refresh else will be as per data type of the object */
    var xDType = xeData["xd_" + xid];
    switch(xc) 
    {
      case valmap.label:
        if(ent.nextSibling != null)
        {
          ent.value = ent.nextSibling.nodeValue;
        }
        //set the copyVal value that was set during setVal
        if(ent.getAttribute("copyVal") != null){
           ent.value = ent.getAttribute("copyVal");
        } 
        break;
          
      case valmap.textbox:
        //Disable the element if it is holding a value to be ignored
        if(ignoreVals != null && 
           contains(ignoreVals, ent.nextSibling.value)){
           xuiPreserveValue(ent, "disabled", ent.disabled);
		   xuiPreserveValue(ent.nextSibling, "disabled", ent.nextSibling.disabled);
           ent.nextSibling.disabled=ent.disabled=true;
           break;
        }
        if(xValue != null){
           ent.value = xValue;
        }      
        else{
           ent.value = ent.nextSibling.value;                
        }        
        if(ent.disabled == true)
        {                
          diabled = true;               
        }                           
        if(ent.value != null && validate == true && diabled == false)
        {
          if(isElementInScopeOfButtonClicked(bid,ent))  
          {              
             //eleName = ent.parentNode.previousSibling.previousSibling.innerHTML;
             if(xuiValidateElement(xid, ent.value)== false)
             {                    
                value = false;
             }
          }                    
        }           
        break;
            
      case valmap.textarea:      
        //Ignore the optional field
        if(ignoreVals != null && 
           contains(ignoreVals, ent.nextSibling.value)){
           xuiPreserveValue(ent, "disabled", ent.disabled);
		   xuiPreserveValue(ent.nextSibling, "disabled", ent.nextSibling.disabled);        
           ent.nextSibling.disabled=ent.disabled=true;
           break;
        }
        
        ent.value = ent.nextSibling.value;    
        if(ent.disabled == true)
        {                
          diabled = true;
        }               
        if(ent.value != null && validate == true && diabled == false)
        { 
          if(isElementInScopeOfButtonClicked(bid,ent))
          {
            //eleName = ent.parentNode.nextsibiling.innerHTML;
            if(xuiValidateElement(xid, ent.value)== false)
            {                    
               value = false;
            }
          }    
        }        
        break;            
            
      case valmap.passwdbox:
        //Ignore the optional field
        if(ignoreVals != null && 
           contains(ignoreVals, ent.nextSibling.value)){
           xuiPreserveValue(ent, "disabled", ent.disabled);
		   xuiPreserveValue(ent.nextSibling, "disabled", ent.nextSibling.disabled);        
           ent.nextSibling.disabled=ent.disabled=true;
           break;
        }
        
        ent.value = ent.nextSibling.value;            
        break;
            
      case valmap.radio:            
      case valmap.radio_left:
      
        if(xValue != null){
           ent.value = xValue;
           break;
        }
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
            ent.value = node.value;
            break;
          }
          else if(isRadioElement(xid)){
            ent.disabled=true;
          }
          node = node.nextSibling;
        }
        break;
            
      case valmap.radio_none:
      
        if(xValue != null){
           ent.value = xValue;
           break;
        }
      
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
           if(xValue != null){
              ent.value = xValue;
              break;
           }
      
            var selected_index = ent.nextSibling.selectedIndex;
            if(xDType == "refresh"){           
               var ev =   getAllRefProfiles(xid);
            }
            else{             
               var ev = getAllWebEnums(xid) ;
            }   
            var item_count     = ev == null?0:ev.length;
            var ea_indices     = xtn;
            
            // disable the elements currently holding the extn action item
            if(contains(ea_indices,selected_index)){
			   xuiPreserveValue(ent, "disabled", ent.disabled);
		       xuiPreserveValue(ent.nextSibling, "disabled", ent.nextSibling.disabled);
               ent.nextSibling.disabled=true; // check if we really need this????
               ent.disabled = true;
            }
            else if(ev != null && selected_index <= ev.length-1)
            {                
               ent.value = ev[selected_index];
            }
            break;
            
      case valmap.combo_multiselect:
        if(xValue != null){
           ent.value = xValue;
           break;
        }
      
        if(ent.nextSibling.xvalue != undefined) 
        {
          ent.value = ent.nextSibling.xvalue;
        }
        //ent.value = ent.nextSibling.value;                     
        break;
            
      case valmap.checkbox:
        if(xValue != null){
           ent.value = xValue;
           break;
        }
      
        if(xDType == "refresh"){           
           var ev =   getAllRefProfiles(xid);
         }
         else{             
             var ev = getAllWebEnums(xid) ;
         }   
        if(ent.nextSibling.checked == true) 
        {
          ent.value = ev[0];
        } 
        else 
        {
          ent.value = ev[1];
        }
        break;
            
      case valmap.checkboxNeg:
        if(xValue != null){
           ent.value = xValue;
           break;
        }
      
        if(xDType == "refresh"){           
           var ev =   getAllRefProfiles(xid);
         }
         else{             
             var ev = getAllWebEnums(xid) ;
         }   
        if(ent.nextSibling.checked == true) 
        {
          ent.value = ev[1];
        } 
        else 
        {
          ent.value = ev[0];
        }
        break;     
            
      case valmap.button:
        break;
        
      case valmap.url:
        break;  
        
      case valmap.file:
        //ent.value = ent.name + "_" + xuiCommonGetBaseName(ent.nextSibling.value);
        break;        

      case valmap.hidden:
        if(xValue != null){
           ent.value = xValue;
        }
      
        break;
                    
      default:
        alert("xuiPageSubmitRender: unknown control : '" + xc + "' for element '" + ent.name + "'");
        break;
    }
        
    if(value == false)
    {
      return false;
    }      
    if(pageRedirect){
       xuiMayBeCopyToRedirectionElement(xid,ent);
    }   
  }
  return true;
}
   
function xuiMayBeCopyToRedirectionElement(xid,ent)   {
   // if(ent.getAttribute('redirect') == null) ent.disabled=true;
    if(ent.getAttribute('redirectTo') == null){
       return;
    }   
    var redirectionElem = document.getElementById("r_" + xid);
    if(redirectionElem == null) return;
    redirectionElem.value = ent.value;
    redirectionElem.disabled=false;
}
  
 
function xuiPageSubmit(form, submitFlag, bid, submitEndFunc,isRedirection)
{
  var isValid = true;              
  var userFlag = true;

  if(submitFlag == xui_operation_submit)
  {
    isValid = xuiPageSubmitRender(true,bid,false);
        
  }
  else if(isRedirection)
  {
    xuiPageSubmitRender(false,bid,true);
  }
  else
  {
    xuiPageSubmitRender(false,bid,false);
  }

  if(submitEndFunc != null)
  {
     userFlag = xuiRunJSFunction(submitEndFunc, submitFlag);
  }

  if((userFlag==null || userFlag == true) && isValid == true)
  {
    if(form != null)
    {  
	  xuiClearPreserved();
      form.submit();
    }
  }
  else
  {
	xuiRevertPreserved();
    xuiClearPreserved();	
  }
}
                

function isEnterKey(e)
{
 var keynum;
 if(window.event) // IE
  {
    keynum = e.keyCode;
  }
  else if(e.which) // Netscape/Firefox/Opera
  {
    keynum = e.which;
  }
  if(keynum == ENTER_KEY_VALUE){
       return false;
    }
}
function isElementInScopeOfButtonClicked(bid,ent){
    
    if(bid == null){
       return false;
    }
      
    var valList = xeData["xbScope_" + bid];
    var xid = ent.getAttribute('xid'); 
    if(valList == null)
    {        
        return true;
    }    
    for(var i=0; i<valList.length; i++){
     if(xid == valList[i]){
         return true;
     }           
    }
    return false;    
}
