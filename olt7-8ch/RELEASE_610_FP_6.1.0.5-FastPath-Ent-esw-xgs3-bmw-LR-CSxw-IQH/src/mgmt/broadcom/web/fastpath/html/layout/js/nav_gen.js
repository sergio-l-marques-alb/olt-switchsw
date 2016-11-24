
var sideNavDiv = null;
var sideNavHtml = "";
var topNavHtml = "";
var localIndex = 0;
var localNodes = null;
var foundNode = null;
var foundTab = null;

function _init_vars(n) {
  sideNavHtml = "";
  topNavHtml = ""
  localNodes = n;
  localIndex = 0;
}

function _find_node(nodes, thisIndex) {
  nodes.each(function(n) {
    
    if(foundNode != null)
      return false;
    
    if(thisIndex == n.nodeIndex) {
      foundNode = n;
      return false;
    }
    
    if (n.children) {
      _find_node(n.children, thisIndex);
    } 
    return true;    
  }.bind(this));
  
}

function _load_page(obj, nodeIndex) {
  var n = xuiFindNode(nodeIndex);
  
  if(n.id == 'tab') {
    //show only this noode children
    sideNavHtml = "";    
    _load_other_nodes(n.children, n);
    $(sideNavDiv).innerHTML = sideNavHtml;
  }else if(n.id == 'tree') {
    // expand or collapse the tree
  }else {
    // no specific action
  }
  
  return xuiLoadPage(obj, n, n.link);
}

function _load_tab_nodes(nodes, parent) {
  
  nodes.each(function(n) {
    
    // set the parent
    if(n.nodeIndex == 'undefined' || n.nodeIndex == null) {    
      n.nodeIndex = localIndex++;
      n.parent = parent;      
    }

    if(n.id == 'tab') {
      topNavHtml = topNavHtml + "<li><a href='#' onClick=_load_page(this," + n.nodeIndex + ");>" + n.name + "</a>";
      if(foundTab == null) foundTab = n;
    }
  
    if (n.children) {
      _load_tab_nodes(n.children, n);
    } 

    if(n.id == 'tab') {
      topNavHtml = topNavHtml + "</li>";
    }
    
  }.bind(this));
}
var count = 1;
function _load_other_nodes(nodes, parent) {
  
  var parent = "somthing"+count;
  var parentDummy = "somthing"+(count-1);
  if ( count == 1 )
  {
     parentDummy = "foldersTree";
	}
  nodes.each(function(n) {
    
    // set the parent
    if(n.nodeIndex == 'undefined' || n.nodeIndex == null) {    
      n.nodeIndex = localIndex++;
      n.parent = parent;      
    }

    if(n.id == 'tree') {
		if(n.children.length > 0){
      //sideNavHtml = sideNavHtml + "<li><a href='#' onClick=_load_page(this," + n.nodeIndex + ");>" + n.name + "</a>";
      //sideNavHtml = sideNavHtml + "<ul>";
    if (n.link != '') {
		count++;
		sideNavHtml = sideNavHtml + "somthing"+(count-1)+ "= insFld("+parentDummy+", gFld(\""+n.name+"\",\"javascript:parent.op()\"));" 
    }
	   
      }
      else {
    if (n.link != '') {
			sideNavHtml = sideNavHtml + "insDoc("+parentDummy+",gLnk(\"R\",\""+n.name+"\",\""+n.link+"\"));"
    }
				
         //sideNavHtml = sideNavHtml + "<li><a href='#' onClick=_load_page(this," + n.nodeIndex + ");>" + n.name + "</a>";
      }
   }
    if (n.children) {
      _load_other_nodes(n.children, n);
    } 

    //if(n.id == 'tree') {
      //sideNavHtml = sideNavHtml + "</ul>";      
      //sideNavHtml = sideNavHtml + "</li>";
    //}
    //else {
    //}
      //sideNavHtml = sideNavHtml + "</li>";
    
  }.bind(this));
}


function xuiLoadNavSmart(topNav, sideNav, data) {
  _init_vars(data.nodes);
  //foundTab = null;
  //_load_tab_nodes(data.nodes, null);
  //$(topNav).innerHTML = topNavHtml; 
  _load_other_nodes(data.nodes, null);
  //sideNavDiv = $(sideNav);
  //$(sideNav).innerHTML = sideNavHtml;
  return sideNavHtml;
}

function xuiFindNode(thisIndex) {
  foundNode = null;
  _find_node(localNodes, thisIndex);
  return foundNode;
}

function generateMenu(rent,something,data)
{
	return xuiLoadNavSmart(null,"tree",xuiNavData);
}
