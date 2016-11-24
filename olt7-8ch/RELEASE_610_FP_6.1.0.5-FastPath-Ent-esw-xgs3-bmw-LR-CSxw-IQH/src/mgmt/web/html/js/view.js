
function view_get_prefix(inst, name, suffix)
{
  var prefix = inst.substring(0,inst.indexOf(name));
  alert("inst " + inst + " name " + name + " prefix " + prefix);
  return prefix;
}

function view_data ()
{
  this.iterName = null;
  this.info = null;
}

function view_build ()
{
  var argv = view_build.arguments;
  var off = 0;

  // create new view data
  var vdata = new view_data();

  // copy the iterator name
  vdata.iterName = argv[off++];

  // copy the rest of the info
  var count = argv.length - off;
  vdata.info = new Array(count);
  for(var i=0; i< count; i++)
  {
    vdata.info[i] = argv[i + off];
  }

  // return the data structure
  return vdata;
}

function view_action (elem, data)
{
  var off = 0;
  var argv = data;
  var argc = argv.length;
  var elem = argv[off++];
  var myform = elem.form;
  var iterName = argv[off++];
  var prop = argv[off++];
  var count = argv[off++];

  // Generate the property instance name and Get the propery object
  var prefix = elem.name.substring(0,elem.name.indexOf(iterName));
  var propInst = prefix + prop;
  var propElem = myform.elements[propInst];
  //alert("MY NAME " + elem.name + "\nMY PROP " + propElem.name);

  // Get the current image base name
  var searchString = "images/";
  var img = searchString + elem.src.substring(elem.src.indexOf(searchString)+searchString.length);

  // Create images array
  var images = new Array(count);
  for (var i = 0; i < count; i++) images[i] = argv[i + off];

  // shift offset
  off = off + count;  

  // Create alt array
  var alts = new Array(count);
  for (var i = 0; i < count; i++) alts[i] = argv[i + off];

  // shift offset
  off = off + count;  

  // Create value array
  var values = new Array(count);
  for (var i = 0; i < count; i++) values[i] = argv[i + off];

  // Change the icon and propery element value
  for (var i = 0; i < count - 1; i++)
  {
    if(img == images[i])
    {
      elem.src = images[i + 1];
      elem.alt = alts[i + 1];
      propElem.value = values[i + 1];
      propElem.disabled = false;
      //alert("img set -> " + elem.src + " " + elem.alt + " " + propElem.value);
      return false;
    }
  }
  elem.src = images[0];
  elem.alt = alts[0];
  propElem.value = values[0];
  propElem.disabled = false;
  //alert("img set => " + elem.src + " " + elem.alt + " " + propElem.value);
  return false;
}

function view_load(vdata)
{
  var form = document.forms[0];

  // get all form input fields
  formInputs = form.getElementsByTagName("input");
  if(formInputs == 0)
    return;

  // loop through all the input fields
  var len = formInputs.length;
  for (var j = 0; j < len; j++)
  {
    // skip the item if not image
    var elem = formInputs.item(j);
    if(elem.type != 'image')
      continue;

    // found image input - process it
    var prefix = view_get_prefix(elem.name, vdata.iterName, "_image");
    var propInst = prefix + prop;
    var propElem = form.elements[propInst];

    for(var i = 0; i < count; i++)
    {
      //alert("CMP " + values[i] + " " + propElem.value);
      if(values[i] == propElem.value)
      {
        elem.src = images[i];
        elem.alt = alts[i];
        //alert("pair " + elem.name + " " + elem.src);
        break;
      }
    }
  }
}

