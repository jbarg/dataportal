<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html>
<head>
<title>CCLRC Data Portal Facility Manager</title>
<script>

function check(){

var username = document.logon.username.value;
var password = document.logon.password.value;
var lifetime = document.logon.lifetime.value;

if(username == "" ||password == ""){
    alert("Please enter a username, password");
    return false;

}
else return true;

}

function openHelp(){
    window.open("../help/Help.jsp","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");

}
</script>

<title>CCLRC Data Portal Facility Manager - Change Password</title>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

<meta name="robots" content="index, nofollow" />

<meta name="description" content="This site is the dataportal" />

<meta name="keywords" content="Dataportal, metadata, clrc, dl, facilities,Estedi, data, search, Glen Drinkwater, Synchrotron Radiation,British Atmospheric Data Centre, shopping basket" />



<link rel="stylesheet" href="style/mainNN4.css" type="text/css" />

<style type="text/css">

@import url(style/main.css); /*IE and NN6x styles*/

</style>

</head>
<body onload="document.forms['logon'].j_username.focus();">
<table width="100%" cellspacing="3" border="0"><tr>
<td valign="top" width="20%" align="left">


<!--<form name="top-search_form" action="">

<input class="button" onclick="location='../index.html'" type="button"
value="Home page" />

</form>-->
</td>
<td valign="top">
<p id="topheader">Change CCLRC Data Portal Facility Manager's password</p>
<br />
<br />
<br />
<br />
<br />
<hr />
<br />
<br />
<form class="mess" name="logon" method="post" action="servlet/LoginServlet"
onsubmit="return check();">
<table cellspacing="0" border="0" cellpadding="3">

<tr>
<td width="100">Username</td>
<td><input type="text" name="username" /></td>
</tr>
<tr>
<td align="left">Passphrase</td>

<td><input type="password" name="password" /></td>

</tr>
<tr><td><input type="submit" value="Login" /></td></tr>
</table>
</form>
<br />
<br />
<br />
<br />
<hr />
<br />

<div id="footer" align="right"> <a href="http://validator.w3.org/check/referer"><img

         src="img/vxhtml.gif"

        alt="Valid XHTML 1.0!" border="0" height="31" width="88" /></a>

<br />
<br />
<a href="http://www.clrc.ac.uk/Activity/ACTIVITY=Copyright;" target="_top">&copy;
<script language="JavaScript"><!--
document.write(new Date().getYear());
//--></script> CCLRC Data Portal</a></div>
</td>
</tr>
</table>
</body>

</html>