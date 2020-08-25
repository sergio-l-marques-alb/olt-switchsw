
function formSubmit_VlanAddRemovePage(submitFlag, thisControl)
{
}

function preLoad_VlanAddRemovePage()
{
  var ids = brcm_getby_tag_and_name("input", "VLANCreateAndDel_VlanIdList_G1N3E1", true);

  for(var i = 0; i < ids.length; i++)
  {
    if(ids[i].value != "1") continue;
    var prefix = brcm_get_iter_prefix(ids[i].name);
    var objs = brcm_getby_tag_and_name("input", prefix + ".VLANCreateAndDel_VlanName_G1N3E2", false);
    if(objs.length > 0) objs[0].disabled = true;
    var objs = brcm_getby_tag_and_name("input", prefix + ".VLANCreateAndDel_VlanDelnable_G1N3E3", false);
    if(objs.length > 0) objs[0].nextSibling.disabled = true;
    var objs = brcm_getby_tag_and_name("input", prefix + ".VLANCreateAndDel_VlanNameEnable_G1N3E4", false);
    if(objs.length > 0) objs[0].nextSibling.disabled = true;
  }
}
