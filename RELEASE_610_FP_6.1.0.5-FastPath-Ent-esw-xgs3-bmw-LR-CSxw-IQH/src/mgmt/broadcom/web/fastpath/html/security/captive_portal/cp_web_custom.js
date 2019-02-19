
var CLEAR = 1;
var DELETE = 2;
var CP_PREVIEW = 11; // CP_AUTH_STATUS_FLAG_t
var DISABLE_COLOR = "gray";
var cpVisual = "";
var cpValue = "";
var ipVisual = "";
var ipValue = "";
var NO_SELECTION = "< No Selection >";
var AUTH_PAGE = "Authentication";
var WELCOME_PAGE = "Welcome";
var LOGOUT_PAGE = "Logout";
var SUCCESS_PAGE = "Success";
var GLOBAL_PAGE = "Global";
var globalNote = "Global parameter";
var logoutDisabledNote = "User Logout feature is disabled";
var logoutEnabledNote = "User Logout feature";
var BRANDING_IMAGE = "BRCM_logo.gif";
var BACKGROUND_IMAGE = "cp_bkg.jpg";
var ACCOUNTING_IMAGE = "login_key.jpg";

var fDirty = false;
var browser = navigator.appName;
var msie = (browser.indexOf("Microsoft") != -1)?true:false;
var defaultDPI = 96; // non-msie browsers
var currentDPI = (msie)?window.screen.logicalXDPI:defaultDPI;

function download()
{
  // because MSIE has no download indicator
  document.body.style.cursor = "wait";
}

function l2h(str)
{     
  var tmp = "";
  var rtn = "";
      
  for (i=0;i<str.length;i++)
  {
    tmp = "0000" + str.charCodeAt(i).toString(16).toUpperCase();
    rtn += tmp.substring(tmp.length-4,tmp.length);
  }
  return rtn;
}

function clearForm()
{
  var clear = window.confirm("NOTE: If you clear this locale configuration,\n"
                          +"all the fields on this page will be reset and\n"
                          +"set to their default values!\n\n"
                          +"Are you sure you want to clear?");
  if (clear)
  {
    setDirty(AUTH_PAGE,false);
    document.forms[0].refresh_in_progress.value = 1;
    document.forms[0].refresh_key.value = CLEAR;
    document.forms[0].submit();
    return true;
  }
  else
  {
    return false;
  }
}

function checkError()
{
  if (document.forms[0].err_flag.value == 1)
  {
    displayErrorMsg();
  }
}

function displayErrorMsg() { alert(document.forms[0].err_msg.value); }

function setDirty(v,b)
{
  fDirty = b;
  if (null != document.getElementById(v))
  {
    document.getElementById(v).disabled = b;
    document.getElementById(v).style.backgroundColor = (b)?DISABLE_COLOR:"";
  }
}

function isDirty(v) { return (fDirty)?true:false; }

