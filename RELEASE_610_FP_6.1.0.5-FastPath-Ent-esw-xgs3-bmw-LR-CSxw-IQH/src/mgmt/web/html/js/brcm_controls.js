
//
// LABEL FUNCTIONS
//
function brcm_load_label()
{
  var tags = brcm_getby_tag_class("input", "hidden_label");

  for (i=0; i<tags.length; i++)
  {
    var label = tags[i].nextSibling;
    label.nodeValue = tags[i].value;
  }
}

//
// CHECKBOX FUNCTIONS
//
function brcm_checkbox_setdisable()
{
  var tags = brcm_getby_tag_class("input", "hidden_checkbox");
  for (i=0; i<tags.length; i++)
  {
    var obj = tags[i];
    if(obj.nextSibling.disabled == true)
    {
      obj.disabled = true;
    }
  }
}

function brcm_checkbox_set_shd(rep, obj, val, s,h,d,e)
{
  obj.value = val;
  if(rep == 0)  // for now don't worry about options for repeat - TODO
  {
    brcm_show_hide_disable_enable(s,h,d,e);
  }
}

function brcm_checkbox(rep, obj, name, en, dis, s1,h1,d1,e1, s2,h2,d2,e2)
{
  if(obj == undefined) //onload
  {
    var tags = brcm_getby_tag_class("input", "hidden_checkbox");
    for (i=0; i<tags.length; i++)
    {
      var obj = tags[i];

      // for onload name is passed and we need to search for this named element
      // while comparing we need to ignore the repeat prefix
      if(brcm_strip_iter_prefix(obj.name) != name) 
      {
        continue;
      }

      if(obj.value == en)
      {
        obj.nextSibling.checked = true;
      }
      else if(obj.value == dis)
      {
        obj.nextSibling.checked = false;
      }

      if(obj.nextSibling.checked == true)
      {
        brcm_checkbox_set_shd(rep, obj, en, s1,h1,d1,e1);
      }
      else if(obj.nextSibling.checked == false)
      {
        brcm_checkbox_set_shd(rep, obj, dis, s2,h2,d2,e2);
      }
    }
  }
  else
  {
    if(obj.checked == true)
    {
      brcm_checkbox_set_shd(rep, obj.previousSibling, en, s1,h1,d1,e1);
    }
    else if(obj.checked == false)
    {
      brcm_checkbox_set_shd(rep, obj.previousSibling, dis, s2,h2,d2,e2);
    }
  }
}

//
// SELECT FUNCTIONS
//
function brcm_select_run_action(obj, name)
{
  if(obj == undefined) return;

  var value = obj.value;

  var argv = brcm_select_run_action.arguments;
  var undefIndex = 0;
  var i = 0;

  for(i = 2; i < argv.length; i+=5)
  {
    if(value == argv[i + 0])
    {
      brcm_show_hide_disable_enable(argv[i + 1], argv[i + 2], argv[i + 3], argv[i + 4]);
      undefIndex = -1;
      break;
    }
    else if(undefined == argv[i + 0])
    {
      undefIndex = i;
    }
  }
  if(undefIndex > 0)
  {
    i = undefIndex;
    brcm_show_hide_disable_enable(argv[i + 1], argv[i + 2], argv[i + 3], argv[i + 4]);
  }
}

function brcm_select_load()
{
  var tags = brcm_getby_tag_class("input", "hidden_select");
  for (i=0; i<tags.length; i++)
  {
    var obj = tags[i];

    // if there are no options create one
    var obj2 = obj.nextSibling;
    var opts = obj2.getElementsByTagName("option");

    if(opts.length == 0)
    {
      var opt = document.createElement("option");
      obj2.appendChild(opt);
      continue;
    }

    // if the hidden value is not set, setit now otherwise use it
    if(obj.value == "")
    {
      obj.value = obj.nextSibling.value;
    }
    else
    {
      obj.nextSibling.value = obj.value;
    }
  }
}

function brcm_select_submit()
{
  var tags = brcm_getby_tag_class("input", "hidden_select");
  for (i=0; i<tags.length; i++)
  {
    tags[i].value = tags[i].nextSibling.value;
  }
}
