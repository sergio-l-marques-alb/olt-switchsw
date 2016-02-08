var pagehasNoRenderDiv = false;
function brcmObjGet(o) 
{
  if (typeof o != 'string') return o;
  return document.getElementById(o);
}
  
function brcmObjSetVisible(o, v) 
{
  var o1 = document.getElementById(o);
  if(o1) o1.style.visibility=v;
}

function brcm_build_img(tag, id)
{
  var img = "<a href='#' "
  img += "onClick='editCSS(";
  img += id;
  img += ")'><img src='images/_css_edit_.png' border=1 alt='Edit CSS'></a>";
  return img;
}

function brcm_show_tag(tag)
{
  var f = MapElements.Get(tag);

  for(var i=0; i<f.length; i++)
  {
    var ent = f[i];
    var cls = ent.getAttribute('class');
    if(cls) 
    {
      ent.parentNode.innerHTML = brcm_build_img(cls) + ent.parentNode.innerHTML;
    }
    else 
    {
      var id = ent.getAttribute('id');
      if(id) 
      {
        ent.parentNode.innerHTML =  brcm_build_img(id) + ent.parentNode.innerHTML;
      }
    }
  }
}

function brcm_common_show_html_tags()
{
  if(parent.parent != parent)
  {
    var func = parent.parent.brcm_show_css_edit_get;
    if(func != undefined && func()) 
    {
      brcm_show_tag('DIV');
      brcm_show_tag('TR');
      brcm_show_tag('TD');
    }
  }
}

//MapElements holds a hashmap in which key is xid/Id and the 
//value is a list of all elements with this xid/id that can be tr/td/input/th/table/div
//Get returns a list of all elements corresponding to an xid/id
//Set adds an entry to the hashmap with xid/id as the key and a list of elements corresponding to this key .
//GetChildElements returns a list containing the element with the given xid/id and it's children.
//
var MapElements = {
  Set : function(xidOrTagNameOrElement,elements_array) {this[xidOrTagNameOrElement] = elements_array;},
 
  Get : function(xidOrTagNameOrElement) {
    
                 var tmp = this[xidOrTagNameOrElement];
                 if(tmp == null)
                    tmp = new Array();
                  
                 return tmp;
  },    
  
  GetElementsByTagNames     : function(tagNames){
    
    var result_array = new Array();
    var tags = tagNames.split(",");
  
    for(var index=0;index<tags.length;index++){
      var tmp_array = MapElements.Get(tags[index]);
      if((tmp_array != null) && (tmp_array.lenght !=0) )     {         
         result_array = concat_array(result_array, tmp_array);
      }
      
    }
    return result_array;
  },
  
  GetChildElementsByXid: function(xid) {
    var children = new Array();
    var arr1 = (MapElements.Get(xid+"_childNodes") == null)? new Array(): MapElements.Get(xid + "_childNodes") ;
       
 
   return arr1;
  } ,
  
  GetElementsByXid : function(xid,tagNames){
      var children = MapElements.GetChildElementsByXid(xid);
      var tmp_array = new Array();
      
      for(var index=0;index<children.length;index++){
          /*need not check containd here as function GetChildElementsByXid checks
          if(tagNames.indexOf(children[index].tagName) != -1 && !contains(tmp_array,children[index]))
             tmp_array.push(children[index]);*/      
         if(tagNames.indexOf(children[index].tagName) !=-1)          
             tmp_array.push(children[index]);      
      }
       
      return tmp_array;
  },
  
  GetRadioElementsByXid : function(xid) {
      var children = MapElements.GetElementsByXid(xid,'INPUT');
      var tmp_array = new Array();
      
      for(var index=0;index<children.length;index++){
          if(children[index].type == "radio" )
             tmp_array.push(children[index]);
      }
      
      return tmp_array;
  },
  GetChildElementsByTagNames : function(tagNames,ent){
      var children = ent.childNodes;
      var resultArray = new Array();
      
      for(var index=0;index<children.length;index++){
          if(tagNames.indexOf(children[index].tagName) != -1){
             resultArray.push(children[index]);
          }
      }
      
      return resultArray;
  },
  
  xuiGetElementsWithPrefix : function(prefix,xid){
      var elements_array = MapElements.GetChildElementsByXid(xid);
      var result_array =   new Array();
    
      //if the element has the prefix, prepare an array with  
      //the elements having the same prefix
  
  
      if(prefix != null){
         for(var index=0;index<elements_array.length; index++)  
         {
             if(elements_array[index].parentNode.getAttribute('p') == prefix)
                result_array.push(elements_array[index]);
                
         }
        
      }  
      
      if(result_array.length == 0) 
         return elements_array;
      else
        return result_array;
     
  },
  GetRedirectionElement : function(xid){
     var elements_array = MapElements.GetElementsByTagNames('INPUT');
     for(var index=0;index<elements_array.length;index++){
         if(elements_array[index].name == "rv_" + xid )
            return elements_array[index];
     }
     
     return null; 
  },
  
  GetNonTabularHiddenInputElementByXid : function(xid){
     var elements  =  document.getElementsByName('v_' + xid);
     if(elements != null && elements.length >0 ){
        return elements[0];
     }
     return null;
  }

  
}//end MapElements   
  



function getChildren_Render(elem,req_tag_names,hashElements){
   var result_array = new Array();
   
   //check the element for norender attribute
   if(elem.getAttribute("norender") != null){
      return result_array;
   }
   
   //check for the children of the element
   for(var index=0;index<elem.childNodes.length;index++){
       var child = elem.childNodes[index];
   
   
       //check for the DOM nodes which are plain text
       if(child.nodeType != 1  ||
          child.getAttribute('norender') != null) continue;


       //check if the child is already traversed
       if(hashElements && child.getAttribute('traversed') == "hashTraversal") continue;
       
       //check if the child is already traversed
       if(!hashElements && child.getAttribute('traversed') == "nonHashTraversal") continue;


       if(hashElements)
          child.setAttribute('traversed', "hashTraversal"); 
       else 
          child.setAttribute('traversed', "nonHashTraversal"); 
        

       //push this child if it has one of the required tag names
       if(req_tag_names.indexOf(child.tagName) != -1){
          result_array.push(child);
          
          if(hashElements)
             hashElement(child);
               
          
       }
       
            
       
       var tmp_array = getChildren_Render(child,req_tag_names,hashElements );
       result_array = concat_array(result_array, tmp_array);
       
       
       //hash the element and it's children by XID
      // hashElement(child);
   }
   
   return result_array;
}


//
//this function returns the elements by collecting them from the divs in a html document
//the divs with attribute 'NORENDER' will not be considred
//
function getDivElements_Render(tagNames,hashElements){
   var div_elements = document.getElementsByTagName('DIV');
   var result_array = new Array();
   
     
   for(var index=0;index<div_elements.length;index++){
       //skip this div if it has NORENDER attribute    
       if(div_elements[index].getAttribute("norender") != null){
          continue;
       }
            
       //collect the children of this div with required tag names  
       var cur_div_children = getChildren_Render(div_elements[index],tagNames,hashElements);
       result_array = concat_array(result_array, cur_div_children);
       
       
   }
   
   return result_array;
}

function getRefreshElement()
{
 var input_elements = document.getElementsByTagName('INPUT');
 for(var index=0;index<input_elements.length;index++){
    if(input_elements[index].getAttribute("refresh")!=null){
        return input_elements[index];
    }
 }
}

function xePageElementRefreshCheck(refEle){
    var ent = refEle;
    var xid = ent.getAttribute('xid');
    if(xid == null){    
        return;
    }    
    var xDType = xeData["xd_" + xid];
     if(xDType == "refresh"){
           var evN =   getAllRefProfiles(xid);
           var evT = getAllRefProfTypes(xid);
           var evR = getAllRefProfRefreshRates(xid);
           var value = ent.value;
           var refreshRate ;
           var type;
           for(var index=0; index<evN.length;index++){
               if(evN[index] == value){
                   type = evT[index];
                   refreshRate = evR[index];
                   if(type == "refresh"){
                       xePageRefreshSet(refreshRate);                       
                   }
               }
           }
     }
     xePageRefresh();
}

function concat_array(arr1,arr2){
      var result_array = arr1;
      for(var index=0;index<arr2.length;index++){
          result_array.push(arr2[index]);
      }
      return result_array;
}

function contains(array,ent){
   for(var index=0;index<array.length;index++){
       if(array[index] == ent)
          return true;
   }
   
   return false;
}

//return the arrya without any duplicate elements
function xuiUniq(array){
  var result_array = new Array();
 
  for(var index=0;index<array.length;index++){
      if(!contains(result_array,array[index])){
         result_array.push(array[index]);
      }
  }
  
  return result_array;
}

//
//Adds the passed element to the list corresponding to xid in MapElements hashmap.
//If there is no list associated with this xid, in MapElements, creates a 
//new entry into hashmap with this xid and element.
//
function fillMap(xidOrTagNameOrElement , element)
{
  var elements_array = MapElements.Get(xidOrTagNameOrElement);
    
  if(elements_array == null)
  {
    elements_array =  new Array();
  }
   
  elements_array[elements_array.length] = element;
  MapElements.Set(xidOrTagNameOrElement,elements_array);
 
  
}

function hashElement(elem){
  
    
  
     
    var id       =(elem.getAttribute('id') == null || elem.getAttribute('id') == "")? 
                 ((elem.getAttribute('xid') == null)? null : 
                   elem.getAttribute('xid')):
                   elem.getAttribute('id');   
     
    //consider the div id if the elem is a div by itself
    
     if(elem.tagName == 'DIV'){
        id       =  elem.getAttribute('div_xid');
     } 
    //
    //cinsider parent Id if the element does not have an Id
    //
    if(id == null && elem.parentNode != null)               {
       id        =(elem.parentNode.getAttribute('id') == null || elem.parentNode.getAttribute('id') == "")? 
                 ((elem.parentNode.getAttribute('xid') == null)? null : 
                   elem.parentNode.getAttribute('xid')):
                   elem.parentNode.getAttribute('id');   

    }

    var xc = xeData['xc_' + id];


//
//Ignore the INPUT element when it's nextSibling is still not generated
//
    if(xc  != "hidden" && elem.type=="hidden" && elem.nextSibling == null)
       return;

 
//  
//1. hash the element by tagName
//
    fillMap(elem.tagName,elem);
  
  

    if(id != null)          {
     
       
//
//2. hash the element by it's TABLE Id          
//
    if(elem.tagName != 'TABLE')
    {     
        var tableId = getTableId(id);
        if(tableId != null)
            fillMap(tableId + "_childNodes",elem);
    }
    
//3. hash the element by it's TR Id          
//
    if(elem.tagName != 'TR' && elem.tagName !='TABLE' )
    {
        var trId = getTrId(id);
        
        if(trId != null)
            fillMap(trId + "_childNodes",elem); 
    }

//4. hash the element by it's TD Id          
//
        
       var tdId = id;
          fillMap(tdId + "_childNodes",elem);  
          
//
//5. hash the element by Id          
//

        fillMap (id,elem);
        
    }   
          

}


function getTableId(xid){
  var tmp = null;
  
  if(xid != null && xid.length>0){
     tmp = xid.substring(0,1);
     if(tmp < 1 || tmp > 9) tmp = null;
     else tmp = xid.split("_")[0];
  }
  
  return tmp;
}

function getTrId(xid){
  var tmp = null;
  
  if(getTableId(xid) != null ){
     var tokens = xid.split("_");
     if(tokens.length >= 2) tmp = tokens[0] + "_" + tokens[1];
     else tmp = null;
  }
   
   return tmp;
}

function getAllWebEnums(xid)
{
  var xe = xeData["xe_" + xid];
  if(xe == null) return null;
  var xe_xid = xe + "_" + xid;
  return  (allWebEnums[xe_xid]== null)? allWebEnums[xe] : allWebEnums[xe_xid];
}
function getAllRefProfiles(xid)
{
    var xRefPro = xeData["xref_" + xid];  
    if(xRefPro == null )return null;
    return allWebRefreshProfiles[xRefPro+"_name"];
}
function getAllRefProfTypes(xid)
{
    var xRefPro = xeData["xref_" + xid];
    if(xRefPro == null )return null;
    return allWebRefreshProfiles[xRefPro+"_type"];
}
function isRadioElement(xid){
  var rGroup= xeData['rGroup'];
  if(rGroup != null && rGroup[0].indexOf(xid) != -1){
     return true;
  }
    
  return false;  
}
function getAllRefProfRefreshRates(xid)
{
    var xRefPro = xeData["xref_" + xid];
    if(xRefPro == null )return null;
    return allWebRefreshProfiles[xRefPro+"_refreshRate"];
}


function getWebOption(xid, opt)
{
  var tmp = "xew_" + xid + "_" + opt;
  return (xeData[tmp]==null) ? opt : xeData[tmp];
}

function getRadioValue(xid){
  var rValue = xeData["xrv_" + xid];
  var ev     = null;
  
  if(rValue == null){
     ev = getAllWebEnums(xid);
     if(ev != null && ev.length == 1) return ev[0];
  }
  return rValue;
}
//
//returns all the child elements of 'obj' with the tag names passed in 'list'
//if obj is null, returns all the elements in document with the tag names passed in 'list'
//the list should be comma-separated string of required tag names
//
function getElementsByTagNames(list,obj) {
  if (!obj) obj = document;
  var tagNames = list.split(',');
  var resultArray = new Array();
  for (var i=0;i<tagNames.length;i++) {
    var tags =  obj.getElementsByTagName(tagNames[i]);
              
     for (var j=0;j<tags.length;j++) {
         resultArray.push(tags[j]);
     }
    // resultArray=resultArray.concat(tags);
    
  }
       
  return resultArray; 
} 

//
//collect all the elements that have xid/id attribute
//and add them to MapXids hashmap by calling the function fillMap()
//so that they can be used for show/hide/disable/enable/setval js actions
//
function collectIds()
{    
  var i, id;

  var f =  getElementsByTagNames('TR,TD,TH,TABLE,CAPTION,INPUT,SELECT,TEXTAREA',null)

  for(i=0; i<f.length; i++)
  {   
    hashElement(f[i]);
  }

}
function checkPageHasNoRenderDiv(){
   var div_elements = document.getElementsByTagName('DIV');
   for(var index=0;index<div_elements.length;index++){
       //skip this div if it has NORENDER attribute    
       if(div_elements[index].getAttribute("norender") != null){
          pagehasNoRenderDiv = true;
          return true;
       }
   }
   return false;
}

function xuiLoadPageHtml()
{
  checkPageHasNoRenderDiv();   
  brcm_common_show_html_tags();  
  // if page has norender div follw traversing thorugh page to collect elements not under no render div
  if(pagehasNoRenderDiv){
      var f = xuiUniq(getDivElements_Render('INPUT', false));
  }else{
      
      var f =getElementsByTagNames('INPUT', null);  
  }    
   xuiProcessOnLoadRadioGroup();
  for(var i=0; i<f.length; i++)
  {
    var nameStr, funcStr, ev, s, j, selected;
    var ent = f[i];
    var xid = ent.getAttribute('xid'); if(xid == null) continue;
    var xc = xeData["xc_" + xid];
    var xp = xeData["xp_" + xid];    
    var xtn = (xeData["xtn_" + xid] == null) ? new Array() : xeData["xtn_" + xid];
    var perm = (xp == valmap.read_only) ? "disabled" : ""
    var doRef = xeData["doRef_" + xid];
    var argHelp=xeData["arghelp_" + xid] == null?"": "&nbsp;&nbsp;" + xeData["arghelp_" + xid];
    var xMaxLen = xeData["xMaxLen_" + xid];
    var xIndex  = xeData["xindex_" + xid];
    var xDType = xeData["xd_" + xid];
    var web_option;
    
    switch(xc) 
    {
      //TODO : for an enum associated with label,textfield,textArea,passwdbox controls,
      //the web_name property should be considered if present.
      //
      case valmap.label:
         var xv;
         
         if(xeData["xindex_" + xid] != null){
            xeData["xindex_" + xid]  = ent.value = xeData["xindex_" + xid]+1;
         }
         //xv=ent.value;
         xv=getWebOption(xid, ent.value);
         xv=trim(xv);
         if(xv.length==0)
           {
               xv="&nbsp;";
           }
        ent.parentNode.innerHTML += xv+ argHelp;
       
        break;
       
     case valmap.textbox: 
     
       if(xMaxLen == null){       
        ent.parentNode.innerHTML += "<input type=text name=" + ent.name  + " " + 
          perm +" onkeypress='return isEnterKey(event)'"  +  " value='" + ent.value + "'>" +  argHelp ;
        
       }else{
           ent.parentNode.innerHTML += "<input type=text name=" + ent.name  + " " + 
          perm + " onkeypress='return isEnterKey(event)'"  +  " value='" + ent.value + "' MAXLENGTH='" + xMaxLen + "'>" +  argHelp ;
       }
       
       
       break;   
            
      case valmap.textarea:
        var s =  "<TEXTAREA cols=16  rows=4  name = " +  ent.name  + " " + 
            perm  + "  >" ;
            s += ent.value + "</TEXTAREA>" + argHelp ;
            
            ent.parentNode.innerHTML += s ;
        break;            

      case valmap.passwdbox:       
       if(xMaxLen == null){  
        ent.parentNode.innerHTML += "<input type=password value='" + ent.value + "'>"+  argHelp;

       }
       else{
           ent.parentNode.innerHTML += "<input type=password value='" + ent.value + "' MAXLENGTH='" + xMaxLen + "'>"+  argHelp;
       }
       break;

      case valmap.radio:
        
        s = "";
        doRef = (doRef == 0) ? false: true;
        nameStr = " name=" + ent.name + " ";
        
       
        var rValue = getRadioValue(xid);
        
        //radio is part of a radio group
        if(isRadioElement(xid)){
                  
         funcStr = " onClick=\"xuiProcessOnChangeRadioGroup(" + 
            doRef +                           // pass on change refresh state
          ", this" + //refernce to this element
          ", '0'" +  //this item's index
          ", '" + xid + "' " +
          ", '" + ent.name + "' " +
          ",'" + valmap.onChange + "'" +
          ");\"";
         
          web_option = getWebOption(xid, (rValue==null)?ent.value:rValue);
          
          if(ent.value == rValue) 
          {            
            s += "<input type=radio value='" + rValue + "' " + funcStr + " " + nameStr + " " +
              perm + " " + xeData["check_" + xid] + " >&nbsp;" + web_option;
          } 
          else if(rValue != null)
          {
            
            s += "<input type=radio value='" + rValue + "' " + funcStr + nameStr + 
              perm + " " + xeData["check_" + xid] + " >&nbsp;" + web_option;
          }
          else 
          {
            s += "<input type=radio value='" + ent.value + "' " + funcStr + nameStr + 
              perm + " " + xeData["check_" + xid] + " >&nbsp;" + web_option;
          }
          
          
          xeData["xrv_" + xid]= (xeData["xrv_" + xid] == null) ? rValue : xeData["xrv_" + xid];
        
        }
        else{
         if(xDType == "refresh"){           
           var ev =   getAllRefProfiles(xid);
         }
         else{             
             var ev = getAllWebEnums(xid) ;
         }    
         for(j=0; ev != null && j<ev.length;j++) 
         {
           funcStr = " onClick=\"xuiProcessOnChangeRefresh(" + 
          "this, " + //refernce to this element  
          doRef +                           // pass on change refresh state
          ", '" + j + "'" +  //this item's index
          ", '" + xid + "' " +
          ", '" + ent.name + "' " +
          ");\"";
         
          web_option = getWebOption(xid, ev[j]);
          if(ent.value == ev[j]) 
          {            
            s += "<input type=radio value='" + ev[j] + "' " + funcStr + nameStr + 
              perm + " CHECKED>&nbsp;" + web_option;
          } 
          else 
          {
            s += "<input type=radio value='" + ev[j] + "' " + funcStr + nameStr + 
              perm +   ">&nbsp;" + web_option;
          }
         }
        
        }
        if(s != "") ent.parentNode.innerHTML += s+argHelp;        
        break;

      case valmap.radio_none:           
        if(xDType == "refresh"){           
           var ev =   getAllRefProfiles(xid);
         }
         else{             
             var ev = getAllWebEnums(xid) ;
         }        
        s = "";        
        prevString ="";
        doRef = (doRef == 0) ? false: true;
        
        nameStr = " name=" + ent.name + " ";
        for(j=0; ev != null && j<ev.length;j++) 
        {
          funcStr = " onClick=\"xuiProcessShedRefreshRadioNone(" + 
            "this, " +                    
            doRef +                       // pass on change refresh state
            ", '" + j + "'" +   //this item's index
            ", '" + xid + "' " +
            ", '" + ent.name + "' " +
            ", '" + ev[j] + "' " +
            ");\"";
        
          web_option = getWebOption(xid, ev[j]);
          if(ent.value == ev[j]) 
          {            
            s += "<input type=radio value='" + ev[j] + "' " + funcStr + nameStr + 
              perm + " CHECKED>&nbsp;" + web_option;
          } 
          else 
          {
            s += "<input type=radio value='" + ev[j] + "' " + funcStr + nameStr + 
              perm + ">&nbsp;" + web_option;
          }            
        }
             
        newIdStr = " id=" + xid+"_prevValue"+" ";
        prevString += "<input type= hidden"+newIdStr+" value= "+ent.value+" >";
        if(s != "") prevString = s + prevString;
        ent.parentNode.innerHTML += prevString +  argHelp;
        break;            
            
      case valmap.radio_left:
        if(xDType == "refresh"){           
           var ev =   getAllRefProfiles(xid);
         }
         else{             
             var ev = getAllWebEnums(xid) ;
         }        
        s = "";
        doRef = (doRef == 0) ? false: true;
        
        nameStr = " name=" + ent.name + " ";
        for(j=0; ev != null && j<ev.length;j++) 
        {
          funcStr = " onClick=\"xuiProcessOnChangeRefresh(" + 
          "this, " + //refernce to this element 
          doRef +                           // pass on change refresh state
          ", '" + j + "'" +   //this item's index
          ", '" + xid + "' " +
          ", '" + ent.name + "' " +
          ");\"";
        
          web_option = getWebOption(xid, ev[j]);
          if(ent.value == ev[j]) 
          {            
            s += web_option + "<input type=radio  value='" + ev[j] + "' " + funcStr + nameStr + 
              perm + " CHECKED>&nbsp;"  ;
          } 
          else 
          {
            s += web_option + "<input type=radio value='" + ev[j] + "' " +  funcStr + nameStr + 
              perm + ">&nbsp;" ;
          }
        }
        if(s != "") ent.parentNode.innerHTML += s + argHelp;        
        break;                 
            
      case valmap.combo:
        if(xDType == "refresh"){           
           var ev =   getAllRefProfiles(xid);
         }
         else{             
             var ev = getAllWebEnums(xid) ;
         }  
        if(ev == null){
          ev= new Array(); 
        }
        doRef = (doRef == 0) ? false: true;
        
            
        funcStr = " onChange=\"" + "xuiProcessOnChangeRefresh(" +
        "this, " + //refernce to this element 
        doRef +                    // pass on change refresh state
        ",this.selectedIndex" +   //this item's index
        ", '" + xid + "' " +       // pass xid
        ", " + "this.name" +       // pass name
        ");\"";
                
        s = "<select " + perm + funcStr + " name=" + ent.name + ">";
           
        selected = 0;
              
        if(ev == null || ev.length == 0)  
        {
          s = s + "<option SELECTED>" + ent.value + "</option>"
        } 
        else 
        {
          selected = 1;
          for(j=0; ev != null && j<ev.length;j++) 
          {
            web_option = getWebOption(xid, ev[j]);
            if(ent.value == ev[j]) 
            {
              s = s + "<option SELECTED>" + web_option + "</option>"
              selected = 2;
            } 
            else 
            {
              s = s + "<option>" + web_option + "</option>"
            }        
          }
        }
        s = s + "</select>"
        if(selected == 1) 
        {
          ent.value = ev[0];
          ent.selectedIndex = 0;
        }
            
        ent.parentNode.innerHTML += s +  argHelp;
        // if the value from the server doesn't match any enum
        // set the value from the server as the first from the combo
        
        
          
        break;

      case valmap.combo_multiselect:
        ev = getAllWebEnums(xid);
        doRef = (doRef == 0) ? false: true;   
        
        //added for testing
        //ent.value="0/8,0/3-0/5";
        //
        var values = xuiTypeDecode(xid, ent.value);                
        
        funcStr = " onChange=\"" + "xuiProcessMultiSelectRefresh(" +
          "this, " + //refernce to this element
          doRef +                    // pass on change refresh state
          ",this.selectedIndex" +   //this item's index
          ", '" + xid + "' " +       // pass xid
          ", " + "this" +  
          ");\"";
                
        s = "<select MULTIPLE size=3" + perm +  funcStr + ">";
           
        selected = 0;
              
        if(ev == null || ev.length == 0)  
        {
          s = s + "<option SELECTED>" + ent.value + "</option>"
        } 
        else 
        {          
          for(j=0; ev != null && j<ev.length;j++) 
          {
            var valStr="value="+ev[j]   ;    
             
            selected = 0;   
            web_option = getWebOption(xid, ev[j]);
            for(var c=0;c<values.length;c++)  {
              if(values[c] == ev[j]) 
              {              
                s = s + "<option " + valStr + "  SELECTED>" + web_option + "</option>"
                selected = 1;
              } 
            }  
            if(selected==0) 
            {
              s = s + "<option " + valStr + ">" + web_option + "</option>"
            }        
          }
        
        }
        s = s + "</select>"
        /* if(selected == 1) 
        {
          ent.value = ev[0];
          ent.selectedIndex = 0;
        }*/
                    
        ent.parentNode.innerHTML += s +  argHelp;
        break;

      case valmap.checkbox:
        if(xDType == "refresh"){           
           var ev =   getAllRefProfiles(xid);
         }
         else{             
             var ev = getAllWebEnums(xid) ;
         }  
        if(ev == null ) {
          break;
        }
        if(ev.length != 2){
           alert("Error! " + ent.name + "  have " + ev.length + " enum options for control " + "checkbox" ) ;
           break;
        }
        
        nameStr = " name=" + ent.name + " ";
        funcStr = " onClick=\"" + "xuiProcessCheckBoxActions(" +
          "this" +
          ", '" + xid + "' " +            // pass xid
        ", '" + ent.name + "' " +       // pass name
        ", '" + ev[0] + "'" +           // first value
        ", '" + ev[1] + "'" +           // second value
        ");\"";            
               
        s = "<input type=checkbox" + nameStr + perm + funcStr;
        if(ent.value == ev[0]) 
        {
          ent.parentNode.innerHTML += s + " CHECKED>" + argHelp;
        } 
        else 
        {               
          ent.parentNode.innerHTML += s + ">" + argHelp;
        }
        break;
            
      case valmap.checkboxNeg:           
        if(xDType == "refresh"){           
           var ev =   getAllRefProfiles(xid);
         }
         else{             
             var ev = getAllWebEnums(xid) ;
         }  
       if(ev == null) {
          break;
       } 
       if(ev.length != 2){
           alert("Error! " + ent.name + "  have " + ev.length + " enum options for control " + "checkbox-Neg" ) ;
           break;
       }
        nameStr = " name=" + ent.name + " ";
        funcStr = " onClick=\"" + "xuiProcessCheckBoxActions(" +
          "this" +
          ", '" + xid + "' " +            // pass xid
        ", '" + ent.name + "' " +       // pass name
        ", '" + ev[1] + "'" +           // first value
        ", '" + ev[0] + "'" +           // second value
        ");\"";            
                
        s = "<input type=checkbox" + nameStr + perm + funcStr;
        
        if(ent.value == ev[1]) 
        {
          ent.parentNode.innerHTML += s + " CHECKED>" + argHelp;
        } 
        else 
        {
          ent.parentNode.innerHTML += s + ">" + argHelp;
        }
        break;
      
      case valmap.button:
        var e = xeData["xt_" + xid];
            
        // prefix from branding        
        s = getButtonPrefix(xeData["xbt_" + xid]); 
        var tmpAction = "";
        if(xeData["xbt_" + xid] == "buttonNone")
        {
          tmpAction = " onClick=\" return formXuiShed(" + 
            "this, " +
            e + " , " +
            "'" +
            xid +
            "'" +
            ");\"";               
        }
        else
        {
          tmpAction = " onClick=\" return onclickSubmit(" + 
            "this, " +
            e + " , " +
            "'" +
            xid +
            "'" +
            "); return false;\"";                               
        }

        // depending on the button style - add url or button
        if(getButtonStyle(xeData["xbt_" + xid]) == "url")
        {      
          s += "<a " + tmpAction + " href=\"#\">" + ent.value + "</a>";
        }
        else 
        {
          s += "<input type=button name=" + ent.name  + " ";             
          s += perm + " value='" + ent.value + "'";
          s += tmpAction + ">";
        }
        
        // suffic from branding                                                            
        s += getButtonSuffix(xeData["xbt_" + xid]);
        
        ent.parentNode.innerHTML += s + argHelp;
        break;

         case valmap.url:
      
          s = "<a name=" + ent.name  + " ";
          s = s + perm + " value='" + ent.value + "'"; 
          s = s + " href = '#'";
          s = s + " onClick=\"xuiProcessJSActions(" + 
            "this, " +
            "'"  + xid + "'"  +
            ", '" + ent.name  + "'" +
            ", '" + ent.value + "'" +
            ", 1" +
            ");\" >" ;
         
           
           
         ent.parentNode.innerHTML += s +  ent.value +  "</a>" + argHelp;
         break;

      case valmap.file:
        //ent.parentNode.innerHTML += "<input type=file name=" + ent.name  + " " + 
        //  perm + " value='" + ent.value + "'>" +  argHelp ;        
        break;

      case valmap.hidden:
        break;
      
      default:
        alert("xuiLoadPageHtml: unknown control : '" + xc + "' for element '" + ent.name + "'");
        break;
    }
  }
  
}



function xuiSwitchPageActions(xid, ent, exec)
{   
  var xc = xeData["xc_" + xid];  
  var ev;   
         
  switch(xc) 
  {
    case valmap.label:
      xuiProcessJSActions(ent,xid, ent.name,ent.value,exec);
      break;

    case valmap.textbox:
       xuiProcessJSActions(ent,xid, ent.name,ent.nextSibling.value, exec);
      break;
            
    case valmap.textarea:
      xuiProcessJSActions(ent,xid, ent.name,ent.nextSibling.value, exec);
      break;            

    case valmap.passwdbox:
      xuiProcessJSActions(ent,xid, ent.name,ent.value, exec);
      break;

    case valmap.radio:
    case valmap.radio_none:
    case valmap.radio_left:
    
      //radio part of radio-group
      //if(isRadioElement(xid)){
      //   xuiProcessOnChangeRadioGroup (false, null, 0, xid, null, exec);
      //}

      var node = ent.nextSibling;
      while(node != null)
      {               
           if(node.checked == true) 
           {
             xuiProcessJSActions(ent,xid, ent.name, node.value, exec);
             break;
           }
           node = node.nextSibling;
      }

      break;

    case valmap.combo:
      var tmp_val = ent.value;
      
      ev = getAllWebEnums(xid);
       
      if(ev != null && ent.nextSibling.selectedIndex >= 0 && 
         ent.nextSibling.selectedIndex < ev.length) {
       
         tmp_val = ev[ent.nextSibling.selectedIndex];
      }
      
      xuiProcessJSActions(ent,xid,ent.name,tmp_val,exec);
      break;

            
    case valmap.combo_multiselect:
      xuiProcessMultiSelectActions(ent,xid, ent.name, ent.nextSibling.options, exec);
      break;

    case valmap.checkbox:
      ev = getAllWebEnums(xid);
      if(ev == null) 
      {
        break;
      }
        
      if(ent.nextSibling.checked == true) 
      {
        xuiProcessJSActions(ent,xid, ent.name, ev[0], exec);
      } 
      else 
      {
        xuiProcessJSActions(ent,xid, ent.name, ev[1], exec);
      }
      break;
            
    case valmap.checkboxNeg:            
      ev = getAllWebEnums(xid);
      if(ev == null) 
      {
        break;
      }
        
      if(ent.value == ev[0]) 
      {
        xuiProcessJSActions(ent,xid, ent.name, ev[0], exec);
      } 
      else 
      {
        xuiProcessJSActions(ent,xid, ent.name, ev[1], exec);
      }
      break;
            
    case valmap.button:
      break;
      
    case valmap.url:
      break;
      
    case valmap.file:
      break;

    case valmap.hidden:
    xuiProcessJSActions(ent,xid, ent.name,ent.value, exec);
      break;
      
    default:
      alert("xuiSwitchPageActions: unknown control : '" + xc + "' for element '" + ent.name + "'");
      break;  
  }
}

function xuiLoadPageActions()
{    
  var f = MapElements.Get('INPUT');
    
  for(var i=0; i<f.length; i++)
  {
    var ent = f[i];
    var xid = ent.getAttribute('xid'); 
    if(xid == null ) continue;
    xuiSwitchPageActions(xid, ent, valmap.onLoad);
  }  
}
function hashDIVElements(){
   var div_elements = document.getElementsByTagName('DIV');      
     
   for(var index=0;index<div_elements.length;index++){
       //skip this div if it has NORENDER attribute    
       if(div_elements[index].getAttribute("norender") != null){
          continue;
       }
            
       //hash the DIV elements with norender tag
       hashElement(div_elements[index]);                    
   }      
}
function xuiLoadPage() 
{
  
  // create dhtml nodes
  xuiLoadPageHtml();
  
 
 //collect and hash elements after generating DOM nodes.
 if(pagehasNoRenderDiv){
     getDivElements_Render('TR,TD,TH,TABLE,CAPTION,INPUT,SELECT,TEXTAREA', true);
 }else {
     collectIds();
 }
  
   //hash the DIV Elements
  hashDIVElements(); 

    //checkTags();
  // execute the actions
  xuiLoadPageActions();
  
  //show the page and hide the progress bar
  brcmObjSetVisible('preload', 'hidden');
  brcmObjSetVisible('topdiv', 'visible');  
}

function stripLeading(string,chr) 
{
  var finished = false;
  for (var i = 0; i < string.length && !finished; i++)
  {
    if (string.substring(i,i+1) != chr) 
    {
      finished = true;
    }
  }
  return finished ?  string.substring(i-1) : string;
}

function stripTrailing(string,chr) 
{
  var finished = false;
  for (var i = string.length - 1; i > -1 && !finished; i--)
  {
    if (string.substring(i,i+1) != chr) 
    {
      finished = true;            
    }
  }
  return finished ? string.substring(0,i+2) : string;
}

function stripLeadingTrailing(string,chr) 
{
  return stripTrailing(stripLeading(string,chr),chr)
}

var refreshTimeLeft = -1;

function xePageRefresh()
{
  if(refreshTimeLeft <= 0)
  {
    // refresh is not enabled
    return;
  }

  if (refreshTimeLeft != 1)
  { 
    refreshTimeLeft -= 1;
    setTimeout("xePageRefresh()",1000);
    return;
  }     
  //refresh the page
  formSubmit(xui_operation_submit,null,null);
}
    
function xePageRefreshSet(val)
{
  if(val > 0)
  {
    refreshTimeLeft = val;
  }
  else
  {
    refreshTimeLeft = -1;       
  }
}

function checkTags()
{  
    //var tag ;
    //tag = document.getElementsByTagName("tr");
    var tag = MapElements.Get('TR') ;
    for (var i=0;i < tag.length ; i++)
      {
       len1 = MapElements.GetChildElementsByTagNames("TD",tag[i]);
       len2 = MapElements.GetChildElementsByTagNames("INPUT",tag[i]);
       len3 = MapElements.GetChildElementsByTagNames("TH",tag[i]);
       
       if(len1.length == 0 && len2.length == 0 && len3.length == 0)
           {
               if(tag[i].parentNode == null)
                   continue;
               else
                {               
                  tag[i].parentNode.removeChild(tag[i]);
                   //tag[i].remove();
                  i=i-1;
                }  
           }  
           
      }
}
