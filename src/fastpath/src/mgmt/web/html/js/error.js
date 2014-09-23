
function DisplayErrorMsg()
{
  alert(document.forms[0].err_msg.value);
}

function CheckError()
{
  if (document.forms[0].err_flag.value == 1)
  {
    DisplayErrorMsg();
  }
}

